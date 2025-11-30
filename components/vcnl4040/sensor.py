import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, UNIT_EMPTY, ICON_EMPTY

# C++ binding: esphome::vcnl4040::VCNL4040Component
vcnl4040_ns = cg.esphome_ns.namespace("vcnl4040")
VCNL4040Component = vcnl4040_ns.class_("VCNL4040Component", cg.PollingComponent, i2c.I2CDevice)

CONF_PROXIMITY = "proximity"
CONF_AMBIENT = "ambient"
CONF_PS_CONF1_2 = "ps_conf1_2"
CONF_PS_CONF3_MS = "ps_conf3_ms"
CONF_PS_CANCELLATION = "ps_cancellation"
DEFAULT_ADDR = 0x60  # 96 decimal

DEPENDENCIES = ["i2c"]
AUTO_LOAD = ["sensor"]

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(VCNL4040Component),
            cv.Optional(CONF_PROXIMITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_EMPTY,
                accuracy_decimals=0,
            ),
            cv.Optional(CONF_AMBIENT): sensor.sensor_schema(
                unit_of_measurement="lx",
                accuracy_decimals=1,
            ),
            cv.Optional(CONF_PS_CONF1_2, default=0x0000): cv.uint16_t,
            cv.Optional(CONF_PS_CONF3_MS, default=0x0000): cv.uint16_t,
            cv.Optional(CONF_PS_CANCELLATION, default=0x0000): cv.uint16_t,
        }
    )
    .extend(i2c.i2c_device_schema(DEFAULT_ADDR))
    .extend(cv.polling_component_schema("50ms"))  # provides platform id + update_interval
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_PROXIMITY in config:
        prox = await sensor.new_sensor(config[CONF_PROXIMITY])
        cg.add(var.set_prox_sensor(prox))

    if CONF_AMBIENT in config:
        als = await sensor.new_sensor(config[CONF_AMBIENT])
        cg.add(var.set_als_sensor(als))

    cg.add(var.set_prox_config(
        config[CONF_PS_CONF1_2],
        config[CONF_PS_CONF3_MS],
        config[CONF_PS_CANCELLATION],
    ))
