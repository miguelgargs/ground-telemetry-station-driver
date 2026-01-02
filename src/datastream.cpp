#include "datastream.h"
#include "checksum.h"
#include "crc_extra.h"
#include "mavlink_types.h"
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>

DataStream::DataStream() : raw_msg_buffer{}, mavlink_msg_buffer{} {}
DataStream::~DataStream() {} // TODO: write the disconnection logic here

void DataStream::push_raw_data(const uint8_t *data, size_t length)
{
    std::vector<uint8_t> copied_data(data, data + length);
    this->raw_msg_buffer.insert(
        raw_msg_buffer.end(), // where to insert (end of dequeue)
        copied_data.begin(),  // from where to insert
        copied_data.end()     // until where to insert
    );

    // Process buffer to extract complete messages
    process_raw_buffer();
}

MsgData DataStream::pull_msg()
{
    // This is complicated:
    // When using a deque in FIFO mode:
    // .insert adds stuff at the end
    // so then it is required to use
    // pop_front to extract it from the front
    std::string current_message = mavlink_msg_buffer.front();
    MavlinkVersion mav_version = get_mavlink_version(&current_message);
    size_t payload_len = get_payload_length(&current_message);
    uint32_t msg_id = get_msg_id(&current_message, mav_version);
    std::string payload = get_payload(&current_message, mav_version, payload_len, msg_id);

    std::cout << "=== MAVLink Message ===" << std::endl;
    std::cout << "Version: " << (mav_version == MavlinkVersion::V1 ? "v1" : "v2") << std::endl;
    std::cout << "Payload length: " << payload_len << std::endl;
    std::cout << "Message ID: " << msg_id << std::endl;
    std::cout << "Payload (hex): " << payload << std::endl;
    HeartbeatData hdata = get_payload_processed(&payload, msg_id);

    std::string src_sys = get_src_sys(&current_message, mav_version);
    std::string src_comp = get_src_comp(&current_message, mav_version);
    // lastly don't forget to pop the message from the queue
    mavlink_msg_buffer.pop_front();
    return MsgData {
        .src_sys = src_sys,
        .src_comp = src_comp,
        .heartbeatdata = hdata
    };
};

MavlinkVersion DataStream::get_mavlink_version(std::string *current_message)
{
    // Byte 0 in hex string is positions 0-1
    uint8_t version_byte = std::stoul(current_message->substr(0, 2), nullptr, 16);
    if (version_byte == static_cast<uint8_t>(MavlinkVersion::V1))
    {
        return MavlinkVersion::V1;
    }
    else
    {
        return MavlinkVersion::V2;
    }
}

size_t DataStream::get_payload_length(std::string *current_message)
{
    // Byte 1 in hex string is positions 2-3
    return std::stoul(current_message->substr(2, 2), nullptr, 16);
}

HeartbeatData DataStream::get_payload_processed(std::string *payload, uint32_t msg_id)
{
    if (msg_id == 0)
    {
        // heartbeat message
        uint32_t custom_mode =
            std::stoul(payload->substr(0, 2), nullptr, 16) |         // Byte 0
            (std::stoul(payload->substr(2, 2), nullptr, 16) << 8) |  // Byte 1
            (std::stoul(payload->substr(4, 2), nullptr, 16) << 16) | // Byte 2
            (std::stoul(payload->substr(6, 2), nullptr, 16) << 24);  // Byte 3

        std::cout << "msg custom_mode=" << custom_mode << std::endl;

        uint8_t type =
            std::stoul(payload->substr(8, 2), nullptr, 16);
        std::cout << "msg type=" << static_cast<int>(type) << std::endl;
        uint8_t autopilot =
            std::stoul(payload->substr(10, 2), nullptr, 16);
        std::cout << "msg autopilot=" << static_cast<int>(autopilot) << std::endl;
        uint8_t base_mode =
            std::stoul(payload->substr(12, 2), nullptr, 16);
        std::cout << "msg base_mode=" << static_cast<int>(base_mode) << std::endl;
        uint8_t system_status =
            std::stoul(payload->substr(14, 2), nullptr, 16);
        std::cout << "msg system_status=" << static_cast<int>(system_status) << std::endl;
        uint8_t version =
            std::stoul(payload->substr(16, 2), nullptr, 16); 
        std::cout << "msg version=" << static_cast<int>(version) << std::endl;

        return HeartbeatData {
            .custom_mode = custom_mode,
            .type = type,
            .base_mode = base_mode,
            .system_status = system_status,
            .mavlink_version = version,
            .autopilot = autopilot
        };
    }
}

std::string DataStream::get_payload(std::string *current_message, MavlinkVersion mav_version, size_t payload_len, uint32_t msg_id)
{
    // get the payload based in:
    // payload len, mavlink version
    // msg id is added to the parameters in order to correctly retrieve the payload
    size_t header_size;
    if (mav_version == MavlinkVersion::V1)
    {
        header_size = 6;
    }
    else if (mav_version == MavlinkVersion::V2)
    {
        header_size = 10;
    }

    size_t payload_start = header_size * 2;
    size_t payload_hex_len = payload_len * 2;

    std::string payload = current_message->substr(payload_start, payload_hex_len);
    return payload;
}

std::string DataStream::get_src_sys(std::string *current_message, MavlinkVersion mav_version)
{
    // the byte in which src_sys is found depends on the MavLink version used.
    std::string src_sys;
    int byte_index;
    if (mav_version == MavlinkVersion::V1)
    {
        byte_index = 3;
    }
    else if (mav_version == MavlinkVersion::V2)
    {
        byte_index = 5;
    }

    src_sys = std::to_string(std::stoul(current_message->substr(byte_index * 2, 2), nullptr, 16));

    return src_sys;
}

std::string DataStream::get_src_comp(std::string *current_message, MavlinkVersion mav_version)
{
    // the byte in which src_comp is found depends on the MavLink version used.
    std::string src_sys;
    int byte_index;
    if (mav_version == MavlinkVersion::V1)
    {
        byte_index = 4;
    }
    else if (mav_version == MavlinkVersion::V2)
    {
        byte_index = 6;
    }

    src_sys = std::to_string(std::stoul(current_message->substr(byte_index * 2, 2), nullptr, 16));

    return src_sys;
}

uint32_t DataStream::get_msg_id(std::string *current_message, MavlinkVersion mav_version)
{
    if (mav_version == MavlinkVersion::V1)
    {
        // v1: msg_id is 1 byte at position 5
        std::string byte_hex = current_message->substr(5 * 2, 2);
        return std::stoul(byte_hex, nullptr, 16);
    }
    else
    {
        // v2: msg_id is 3 bytes (24-bit) at positions 7, 8, 9 (little-endian)
        uint8_t byte0 = std::stoul(current_message->substr(7 * 2, 2), nullptr, 16);
        uint8_t byte1 = std::stoul(current_message->substr(8 * 2, 2), nullptr, 16);
        uint8_t byte2 = std::stoul(current_message->substr(9 * 2, 2), nullptr, 16);
        return byte0 | (byte1 << 8) | (byte2 << 16);
    }
}

bool DataStream::has_messages()
{
    return !mavlink_msg_buffer.empty();
}

void DataStream::process_raw_buffer()
{
    // Keep processing while we have enough data
    while (raw_msg_buffer.size() >= 8) // Minimum MAVLink v1 message size
    {
        // Look for magic byte (0xFE for MAVLink v1, 0xFD for MAVLink v2)
        if (raw_msg_buffer[0] != 0xFE && raw_msg_buffer[0] != 0xFD)
        {
            // Not a valid start, discard this byte and continue
            raw_msg_buffer.pop_front();
            continue;
        }

        bool is_mavlink_v2 = (raw_msg_buffer[0] == 0xFD);

        // Read payload length (byte 1)
        uint8_t payload_len = raw_msg_buffer[1];

        // Calculate total message size
        size_t msg_size;
        size_t msg_id_offset;

        if (is_mavlink_v2)
        {
            msg_size = 12 + payload_len; // MAVLink v2: 10 header + 2 checksum + payload
            msg_id_offset = 7;           // Message ID starts at byte 7 (24-bit)
        }
        else
        {
            msg_size = 8 + payload_len; // MAVLink v1: 6 header + 2 checksum + payload
            msg_id_offset = 5;          // Message ID at byte 5 (8-bit)
        }

        // Check if we have the complete message
        if (raw_msg_buffer.size() < msg_size)
        {
            // Not enough data yet, wait for more
            break;
        }

        // Extract the message
        std::vector<uint8_t> message_bytes(msg_size);
        for (size_t i = 0; i < msg_size; i++)
        {
            message_bytes[i] = raw_msg_buffer[i];
        }

        // Extract message ID
        int message_id;
        if (is_mavlink_v2)
        {
            // MAVLink v2: 24-bit message ID (little-endian)
            message_id = message_bytes[7] | (message_bytes[8] << 8) | (message_bytes[9] << 16);
        }
        else
        {
            // MAVLink v1: 8-bit message ID
            message_id = message_bytes[5];
        }

        // Get CRC_EXTRA for this message
        int crc_extra = get_crc_extra(message_id);

        // Calculate checksum
        uint16_t calculated_crc;
        crc_init(&calculated_crc);

        // Calculate CRC on header + payload (skip magic byte and checksum bytes)
        for (size_t i = 1; i < msg_size - 2; i++)
        {
            crc_accumulate(message_bytes[i], &calculated_crc);
        }

        // Add CRC_EXTRA if we know it
        if (crc_extra >= 0)
        {
            crc_accumulate((uint8_t)crc_extra, &calculated_crc);
        }

        // Extract received checksum (little-endian)
        uint16_t received_crc = message_bytes[msg_size - 2] | (message_bytes[msg_size - 1] << 8);

        // Validate checksum
        bool crc_valid = (crc_extra >= 0) && (calculated_crc == received_crc);

        if (crc_valid)
        {
            // Convert message to hex string for transmission
            std::stringstream ss;
            for (size_t i = 0; i < msg_size; i++)
            {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)message_bytes[i];
            }

            // Add the validated message to the processed buffer
            mavlink_msg_buffer.push_back(ss.str());
        }
        else
        {
            // CRC failed or unknown message type - discard first byte and try again
            // This helps resync if we get corrupted data
            raw_msg_buffer.pop_front();
            continue;
        }

        // Remove the processed message from raw buffer
        for (size_t i = 0; i < msg_size; i++)
        {
            raw_msg_buffer.pop_front();
        }
    }
}
