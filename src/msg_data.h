#include <cstdint>
#include "heartbeat_data.h"
#ifndef MSG_DATA_H
#define MSG_DATA_H
struct MsgData
{
    std::string src_sys;
    std::string src_comp;
    HeartbeatData heartbeatdata;
};
#endif
