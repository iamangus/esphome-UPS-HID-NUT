#include "components/ups_hid/apc_report_values.h"
#include "components/ups_hid/usb_descriptor_utils.h"
#include <cassert>
#include <cmath>

using namespace esphome::ups_hid;

int main() {
  const uint8_t summary[] = {0x09, 0x4C, 0x0A};
  const uint8_t battery[] = {0x26, 0x67, 0x0A};
  const uint8_t discharging_voltage[] = {0x09, 0x5C, 0x09};
  assert(std::fabs(apc_report::battery_voltage(summary, 3) - 26.36f) < 0.001f);
  assert(std::fabs(apc_report::battery_voltage(battery, 3) - 26.63f) < 0.001f);
  assert(std::fabs(apc_report::battery_voltage(discharging_voltage, 3) - 23.96f) < 0.001f);
  assert(std::isnan(apc_report::battery_voltage(summary, 2)));
  const uint8_t nominal120[] = {0x30, 0x78};
  const uint8_t nominal230[] = {0x30, 0xE6, 0};
  assert(apc_report::nominal_input_voltage(nominal120, 2) == 120);
  assert(apc_report::nominal_input_voltage(nominal230, 3) == 230);
  assert(std::isnan(apc_report::nominal_input_voltage(nominal120, 1)));
  const uint8_t runtime_low[] = {0x24, 0x78, 0};
  assert(apc_report::runtime_low_minutes(runtime_low, 3) == 2);
  assert(std::isnan(apc_report::runtime_low_minutes(runtime_low, 2)));
  const uint8_t online[] = {0x06, 1, 0, 0x08};
  const uint8_t on_battery[] = {0x06, 0, 1, 0x10};
  uint8_t flag = 0;
  assert(apc_report::status_flag(online, 4, flag) && flag == 0x08);
  assert(apc_report::status_flag(on_battery, 4, flag) && flag == 0x10);
  assert(!apc_report::status_flag(on_battery, 3, flag));

  // Composite device: skip a different interface and its HID descriptor.
  const uint8_t config[] = {
      9, 2, 45, 0, 2, 1, 0, 0x80, 50,
      9, 4, 0, 0, 1, 3, 0, 0, 0,
      9, 0x21, 0x11, 1, 0, 1, 0x22, 0x10, 0,
      9, 4, 1, 0, 1, 3, 0, 0, 0,
      9, 0x21, 0x11, 1, 0, 1, 0x22, 0x34, 0x05,
  };
  assert(hid_report_descriptor_length(config, sizeof(config), 0) == 16);
  assert(hid_report_descriptor_length(config, sizeof(config), 1) == 0x534);
  assert(hid_report_descriptor_length(config, sizeof(config), 2) == 0);
  for (size_t size = 0; size < sizeof(config); size++) {
    assert(hid_report_descriptor_length(config, size, 1) == 0);
  }
  const uint8_t malformed[] = {0, 2};
  assert(hid_report_descriptor_length(malformed, sizeof(malformed), 0) == 0);
  const uint8_t truncated_entries[] = {
      9, 4, 0, 0, 1, 3, 0, 0, 0,
      6, 0x21, 0x11, 1, 0, 2,
  };
  assert(hid_report_descriptor_length(truncated_entries, sizeof(truncated_entries), 0) == 0);
}
