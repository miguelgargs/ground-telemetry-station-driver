#include <map>
#include <string>
#include <vector>

std::map<int, std::string> messageMap = {
    {0, "HEARTBEAT"}, {1, "SYSTEM_STATUS"}, {2, "SYSTEM_TIME"}};

std::string getMessageName(int type) {
  auto it = messageMap.find(type);
  if (it != messageMap.end()) {
    return it->second; // qué hace esto??
  }
  return "UNKNOWN_MESSAGE";
};

uint16_t getMessageCrc(const char *buffer, const int size, const int crc_extra) {
  /**
  Calculate the CRC and return it. Requires the
  CRC_EXTRA!
   */
  // skip Magic Byte (at the beginning)
  // MAVLink2 supposedly skips all leading zeroes.
  // i have no clue, so i will just skip that part
  // and see what happens when i try to validate
  // some CRCs.
  // char localBuffer = char[1024];
  uint16_t crc = 0xFFFF;

  for (int i=1; i < size-2; i++) { // skip magic bytes (0xFD)
    unsigned char b = (unsigned char) buffer[i];

    uint16_t tmp = b ^ (crc & 0xFF);
    tmp = (tmp ^ (tmp << 4)) & 0xFF;
    crc = (crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4);
  }

  // final XOR with crc_extra
  crc ^= crc_extra;

  return crc;
}

// MAVLink V2 Frame Format:
// Byte 0: 0xFD (start marker)
// Byte 1: payload_length (0-255)
// Bytes 2-3: flags + incompatibility_flags
// Bytes 4: sequence
// Bytes 5-6: system_id, component_id
// Bytes 7-9: message_id (24-bit little-endian)
// Bytes 9+: payload (variable length)
// Last 2 bytes: checksum (CRC-16)

struct SystemStatus {
  uint32_t onboard_control_sensors_present;
  uint32_t onboard_control_sensors_enabled;
  uint32_t onboard_control_sensors_health;
  uint16_t load;
  uint16_t voltage_battery;
  int16_t current_battery;
  int8_t battery_remaining;
  uint16_t drop_rate_comm;
  uint16_t errors_comm;
  uint16_t errors_count1;
  uint16_t errors_count2;
  uint16_t errors_count3;
  uint16_t errors_count4;
  // MAVLink V2 extensions:
  uint32_t onboard_control_sensors_present_extended;
  uint32_t onboard_control_sensors_enabled_extended;
  uint32_t onboard_control_sensors_health_extended;
};
