from esphome.components import number
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import pins
from esphome.const import (
    CONF_ID,
    CONF_PINS,
)

DEPENDENCIES = []
CODEOWNERS = ["@lylavoie"]

CONF_COUNT_BACK = "count_back"
CONF_COUNT_BACK_SPEED = "count_back_speed"

nixie_number_ns = cg.esphome_ns.namespace("nixie_number")
NixieNumber = nixie_number_ns.class_(
    "NixieNumberComponent", number.Number, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    number.number_schema(NixieNumber)
    .extend(
        {
            cv.Required(CONF_PINS): cv.All(
                [pins.gpio_output_pin_schema],
                cv.Length(min=1, max=10),
            ),
            cv.Optional(CONF_COUNT_BACK, default=False): cv.boolean,
            cv.Optional(CONF_COUNT_BACK_SPEED, default=20): cv.int_range(
                min=1, max=100
            ),
        }
    )
    .extend(cv.polling_component_schema("60s")),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await number.register_number(
        var, config, min_value=0, max_value=len(config[CONF_PINS]) - 1, step=1
    )
    pins = []
    for pin in config[CONF_PINS]:
        pins.append(await cg.gpio_pin_expression(pin))
    cg.add(var.set_pins(pins))
    cg.add(var.set_count_back(config[CONF_COUNT_BACK]))
    cg.add(var.set_count_back_speed(config[CONF_COUNT_BACK_SPEED]))
