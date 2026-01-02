#include <cstdint>
#ifndef HEARTBEAT_DATA_H
#define HEARTBEAT_DATA_H
struct HeartbeatData
{
    uint32_t custom_mode;
    uint8_t type;
    uint8_t autopilot;
    uint8_t base_mode;
    uint8_t system_status;
    uint8_t mavlink_version;
};
#endif
