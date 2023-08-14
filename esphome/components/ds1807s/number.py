import esphome.codegen as cg
from esphome.components import i2c, number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_ADDRESS,
    CONF_ZERO,
)

CONF_WIPER = "wiper"

CODEOWNERS = ["@lylavoie"]
DEPENDENCIES = ["i2c"]

ds1807s_ns = cg.esphome_ns.namespace("ds1807s")
DS1807S_component = ds1807s_ns.class_(
    "DS1807Scomponent",
    number.Number,
    cg.PollingComponent,
    i2c.I2CDevice,
)

DS1807Swiper = ds1807s_ns.enum("DS1807Swiper")
CONF_WIPER_ENUM = {
    "pot0": DS1807Swiper.POT0,
    "pot1": DS1807Swiper.POT1,
    "both": DS1807Swiper.BOTH,
}

CONFIG_SCHEMA = cv.All(
    number.number_schema(DS1807S_component)
    .extend(
        {
            cv.Optional(CONF_WIPER, default="both"): cv.enum(
                CONF_WIPER_ENUM, lower=True
            ),
            cv.Optional(CONF_ZERO, default=True): cv.boolean,
        }
    )
    .extend(i2c.i2c_device_schema(0x28))
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    await number.register_number(
        var, 
        config,
        min_value=0,
        max_value=64,
        step=1)
    cg.add(var.set_wiper(config[CONF_WIPER]))
    cg.add(var.set_zero_crossing(config[CONF_ZERO]))
