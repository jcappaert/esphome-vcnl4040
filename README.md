# esphome-vcnl4040

ESPHome external component for the VCNL4040 proximity + ambient light sensor.

## Features
- Proximity and ambient light sensors exposed as ESPHome sensors.
- Configurable proximity registers (PS_CONF1_2, PS_CONF3_MS, PS_CANC) for covers/filters.
- Works as a drop-in ESPHome platform component.

## Installation

### As a git source (recommended)
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/jcappaert/esphome-vcnl4040
    components: [vcnl4040]
```

### As a local source
```yaml
external_components:
  - source:
      type: local
      path: /path/to/esphome-vcnl4040/components/
    components: [vcnl4040]
```

## Wiring (ESP32 example)
- VCC → 3.3V
- GND → GND
- SDA → GPIO21 (or your chosen SDA)
- SCL → GPIO22 (or your chosen SCL)
- INT → optional (not used by this driver)

## Minimal YAML example
```yaml
i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true

sensor:
  - platform: vcnl4040
    address: 0x60
    update_interval: 50ms
    proximity:
      name: "Proximity"
    ambient:
      name: "Ambient Light"
```

## Proximity tuning (covers/filters)
You can tweak the proximity engine to punch through IR windows and remove DC offset.

```yaml
sensor:
  - platform: vcnl4040
    address: 0x60
    update_interval: 25ms
    ps_conf1_2: 0x0A20      # LED current/duty/integration/persistence (datasheet fields)
    ps_conf3_ms: 0x0004     # LED current / sunlight cancel / multi-sampling (datasheet fields)
    ps_cancellation: 0x0020 # Offset to subtract window baseline
    proximity:
      name: "Proximity"
    ambient:
      name: "Ambient"
```

**Guidance:**
- Start from defaults (`0x0000` for all) to confirm the sensor works.
- Increase TX/integration: bump `ps_conf1_2` to stronger settings (e.g., `0x0820`, `0x0A20`, `0x0C20`); see datasheet for exact bit meanings.
- Raise LED current via `ps_conf3_ms` lower bits (e.g., `0x0004`, `0x0008`). Avoid setting shutdown/sunlight-cancel bits unless needed.
- Offset/cancellation: add `ps_cancellation` (e.g., `0x0020`–`0x0100`) only if idle readings sit high due to a window; too much will kill range.
- Change one register at a time and watch raw proximity values to avoid saturation.

## Notes
- Default I2C address: `0x60`.
- Ambient readings are scaled by 0.25 (datasheet LSB).
- The driver logs the applied PS config in `dump_config` for verification.

## Troubleshooting
- No response after tweaking registers: revert to all zeros and power-cycle; then increase one register at a time.
- Very close-only detection: increase `ps_conf1_2` and LED current (`ps_conf3_ms`), keep cancellation low.
- Saturation/false triggers: reduce LED current/integration or increase `ps_cancellation` modestly.
