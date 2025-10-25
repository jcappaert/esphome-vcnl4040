#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace vcnl4040 {

static const uint8_t VCNL4040_I2C_ADDRESS = 0x60;
static const char *const TAG = "vcnl4040";


// Registers (per datasheet)
static const uint8_t REG_ALS_CONF     = 0x00;
static const uint8_t REG_PS_CONF1_2   = 0x03;
static const uint8_t REG_PS_CONF3_MS  = 0x04;
static const uint8_t REG_PS_CANC      = 0x05;
static const uint8_t REG_PS_DATA      = 0x08;
static const uint8_t REG_ALS_DATA     = 0x09;

class VCNL4040Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_prox_sensor(sensor::Sensor *s) { this->prox_sensor_ = s; }
  void set_als_sensor(sensor::Sensor *s)  { this->als_sensor_  = s; }

  void setup() override {
    // Minimal enable; tune config as needed.
    write16_(REG_ALS_CONF, 0x0000);      // ALS enabled
    write16_(REG_PS_CONF1_2, 0x0000);    // PS enabled (defaults)
    write16_(REG_PS_CONF3_MS, 0x0000);
    write16_(REG_PS_CANC, 0x0000);
  }

  void dump_config() override {
    ESP_LOGCONFIG("vcnl4040", "VCNL4040:");
    LOG_I2C_DEVICE(this);
    LOG_UPDATE_INTERVAL(this);
    LOG_SENSOR("  ", "Proximity", this->prox_sensor_);
    LOG_SENSOR("  ", "Ambient", this->als_sensor_);
  }

  void update() override {
    uint16_t v;
    if (this->prox_sensor_ != nullptr) {
      if (read16_(REG_PS_DATA, &v)) this->prox_sensor_->publish_state(static_cast<float>(v));
      else ESP_LOGW("vcnl4040", "Failed reading PS_DATA");
    }
    if (this->als_sensor_ != nullptr) {
      if (read16_(REG_ALS_DATA, &v)) this->als_sensor_->publish_state(v * 0.25f); // rough lux scale
      else ESP_LOGW("vcnl4040", "Failed reading ALS_DATA");
    }
  }

 protected:
  bool write16_(uint8_t reg, uint16_t val) {
    uint8_t data[3] = {reg, static_cast<uint8_t>(val & 0xFF), static_cast<uint8_t>(val >> 8)};
    return this->write(data, 3);
  }
  bool read16_(uint8_t reg, uint16_t *val) {
    uint8_t buf[2];
    auto err = this->write_read(&reg, 1, buf, 2);  // new API
    if (err != i2c::ERROR_OK) return false;
    *val = (static_cast<uint16_t>(buf[1]) << 8) | buf[0];
    return true;
  }

 private:
  sensor::Sensor *prox_sensor_{nullptr};
  sensor::Sensor *als_sensor_{nullptr};
};

}  // namespace vcnl4040
}  // namespace esphome