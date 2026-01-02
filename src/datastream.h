#pragma once
#include "mavlink_types.h"
#include <deque>
#include <cstdint>
#include <cstddef>
#include <string>
#include "heartbeat_data.h"
#include "msg_data.h"

/**
 * DataStream handles and processes messages, and lets
 * DataStreamService send them to the backend through
 * gRPC.
 */
class DataStream
{
private:
    std::deque<uint8_t> raw_msg_buffer;
    std::deque<std::string> mavlink_msg_buffer;

    /**
     * Process raw_msg_buffer and extract valid MAVLink messages
     * into mavlink_msg_buffer
     */
    void process_raw_buffer();

    /**
     * Retrieve Mavlink version
     */
     MavlinkVersion get_mavlink_version(std::string *current_message);

     /**
     * Retrieve payload length
     */
     size_t get_payload_length(std::string *current_message);

    /**
     * @brief Get payload
     * 
     */
    std::string get_payload(std::string *current_message, MavlinkVersion mav_version, size_t payload_len, uint32_t msg_id);

    /** Get src_sys
     */
    std::string get_src_sys(std::string *current_message, MavlinkVersion mav_version);

    /** Get src_comp
     */
    std::string get_src_comp(std::string *current_message, MavlinkVersion mav_version);

    /**
     * @brief Get message id.
     * Type is uint32_t so that both versions (v1, v2)
     * fit in the same message.
     * 
     */
    uint32_t get_msg_id(std::string *current_message, MavlinkVersion mav_version);
    
    /**
     * Get payload from message with its message id
     */
    HeartbeatData get_payload_processed(std::string *payload, uint32_t msg_id);

public:
    /**
     * Default constructor
     */
    DataStream();

    /**
     * Default destructor
     */
    ~DataStream();

    /**
     * Pushes data (raw mavlink message as bytes)
     * into the raw messages buffer. Needs to know the length
     * in order to be able to copy the data into the structure.
     */
    void push_raw_data(const uint8_t *data, size_t length);

    /**
     * Retrieve message from the MAVLink messages buffer.
     */
    MsgData pull_msg();

    /**
     * Check if the MAVLink messages buffer is empty or not.
     */
    bool has_messages();

};
