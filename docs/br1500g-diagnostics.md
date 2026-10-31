# BR1500G diagnostics over Wi-Fi

The UPS remains connected to the ESP32. No PC USB connection is needed.
See `configs/examples/br1500g-xiao.yaml` for a XIAO ESP32-S3 configuration.

## Installation and descriptor capture

1. Publish these changes to the fork and point `external_components` at that
   revision. A local checkout is not automatically visible to a cluster build.
2. Keep the existing Wi-Fi, OTA password, node name and API encryption key.
   Install the firmware over Wi-Fi using the ESPHome dashboard.
3. Open network Logs at INFO or DEBUG level.
4. Press **Dump UPS USB Descriptor** in Home Assistant once. The template
   button calls `id(ups_monitor).dump_usb_descriptors()`.
5. Save `USB identity`, `HID_REPORT_DESCRIPTOR_BEGIN`, every numbered `HID_RD`
   line, and `HID_REPORT_DESCRIPTOR_END`. Retry if log chunks are missing.

The dump issues only a standard GET_DESCRIPTOR request for the claimed HID
interface. It does not issue a UPS shutdown, self-test or configuration write.
The asynchronous transfer remains allocated until its completion callback;
there is no stack completion context that could outlive a timeout.

## Corrections supported by captured reports

- `09 4C 0A`: PowerSummary battery voltage, **26.36 V**, not 263.6 V AC.
- `26 67 0A`: battery voltage, **26.63 V**, valid for a 24 V bank.
- `06 01 00 08` / `06 00 01 10`: charging, discharging, then APC status.
- `30 78`: nominal input voltage **120 V** (one-byte payload is valid).
- `24 78 00`: low-runtime threshold **120 seconds / 2 minutes**.
- `0D 64`: DesignCapacity, not a frequency measurement. Frequency probing is
  disabled, and output voltage/frequency remain unavailable until verified.
- USB manufacturer/product indices come from the actual device descriptor.
  Firmware comes from the product string, not the OEM-information report.
- Test result is read from Battery.Test (`0x21`), not ConfigActivePower (`0x52`).
- Native on-battery/charging entities now use reported state, and unsupported
  numeric data in a successful poll is published as unavailable instead of
  retaining old values.

These are corrections to the existing hardcoded Back-UPS layout, not a general
HID descriptor parser. Other APC families may use different report layouts.
Control/write report mappings have not been validated for this BR1500G; the
example deliberately exposes no UPS control buttons or delay settings.

Run captured-payload and descriptor-boundary tests with:

```sh
pytest tests/test_br1500g_reports.py
```
