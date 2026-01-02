#ifndef MAVLINK_TYPES_H
#define MAVLINK_TYPES_H

#include <cstdint>

enum class MavlinkVersion: uint8_t {
    V1 = 0xFE,
    V2 = 0xFD
};

#endif
