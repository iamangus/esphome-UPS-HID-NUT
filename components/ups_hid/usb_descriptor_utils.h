#pragma once

#include <cstddef>
#include <cstdint>

namespace esphome {
namespace ups_hid {

// Locate the report descriptor belonging to the claimed HID interface, not
// merely the first HID descriptor in a composite device configuration.
inline uint16_t hid_report_descriptor_length(const uint8_t *config, size_t size, uint8_t interface_num) {
  bool selected = false;
  for (size_t offset = 0; offset + 2 <= size;) {
    const uint8_t *desc = config + offset;
    const size_t length = desc[0];
    if (length < 2 || length > size - offset) return 0;
    if (desc[1] == 0x04) {
      selected = length >= 9 && desc[2] == interface_num && desc[3] == 0 && desc[5] == 0x03;
    } else if (selected && desc[1] == 0x21 && length >= 6) {
      for (size_t i = 0; i < desc[5]; i++) {
        const size_t entry = 6 + i * 3;
        if (entry + 3 > length) return 0;
        if (desc[entry] == 0x22) return desc[entry + 1] | (uint16_t(desc[entry + 2]) << 8);
      }
    }
    offset += length;
  }
  return 0;
}

}  // namespace ups_hid
}  // namespace esphome
