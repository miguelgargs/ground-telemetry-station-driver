#pragma once
#include <map>
#include <cstdint>

/**
 * CRC_EXTRA values for MAVLink message validation.
 * These are needed for proper checksum calculation and vary per message type.
 *
 * Source: MAVLink common.xml message definitions
 * https://mavlink.io/en/messages/common.html
 */

static const std::map<int, uint8_t> CRC_EXTRA_MAP = {
    {0, 50},    // HEARTBEAT
    {1, 124},   // SYS_STATUS
    {2, 137},   // SYSTEM_TIME
    {24, 24},   // GPS_RAW_INT
    {30, 39},   // ATTITUDE
    {33, 181},  // GLOBAL_POSITION_INT
    {74, 20},   // VFR_HUD
    {147, 8}    // BATTERY_STATUS
};

/**
 * Get the CRC_EXTRA value for a given message ID
 *
 * @param message_id The MAVLink message ID
 * @return CRC_EXTRA value, or -1 if message ID is unknown
 */
inline int get_crc_extra(int message_id) {
    auto it = CRC_EXTRA_MAP.find(message_id);
    if (it != CRC_EXTRA_MAP.end()) {
        return it->second;
    }
    return -1; // Unknown message ID
}
