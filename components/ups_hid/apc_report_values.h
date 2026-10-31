#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

namespace esphome {
namespace ups_hid {
namespace apc_report {

// Offsets include the report-ID byte. These layouts match the Back-UPS
// reports captured from the BR1500G; HID usage IDs are NOT report IDs.
inline float battery_voltage(const uint8_t *data, size_t size) {
  if (size < 3) return NAN;
  return static_cast<float>(data[1] | (uint16_t(data[2]) << 8)) / 100.0f;
}

inline float nominal_input_voltage(const uint8_t *data, size_t size) {
  if (size < 2) return NAN;
  return static_cast<float>(data[1] | (size >= 3 ? uint16_t(data[2]) << 8 : 0));
}

inline float runtime_low_minutes(const uint8_t *data, size_t size) {
  if (size < 3) return NAN;
  return static_cast<float>(data[1] | (uint16_t(data[2]) << 8)) / 60.0f;
}

inline bool status_flag(const uint8_t *data, size_t size, uint8_t &flag) {
  if (size < 4) return false;
  flag = data[3];
  return true;
}

}  // namespace apc_report
}  // namespace ups_hid
}  // namespace esphome
