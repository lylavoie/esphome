from esphome.components import number
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import pins
from esphome.const import (
    CONF_ID,
)

DEPENDENCIES = []
CODEOWNERS = ["@lylavoie"]

nixie_number_ns = cg.esphome_ns.namespace("nixie_number")
NixieNumber = nixie_number_ns.class_(
    "NixieNumberComponent", number.Number, cg.PollingComponent
)

CONFIG_SCHEMA = cv.All(
    number.number_schema(NixieNumber)
    .extend(
        {
            cv.Required("zero_pin"): pins.gpio_output_pin_schema,
            cv.Required("one_pin"): pins.gpio_output_pin_schema,
            cv.Required("two_pin"): pins.gpio_output_pin_schema,
            cv.Required("three_pin"): pins.gpio_output_pin_schema,
            cv.Optional("count_back", default=False): cv.boolean,
            cv.Optional("count_back_speed", default=20): cv.int_range(min=1, max=100),
        }
    )
    .extend(cv.polling_component_schema("60s")),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await number.register_number(var, config, min_value=0, max_value=3, step=1)
    zero_pin = await cg.gpio_pin_expression(config["zero_pin"])
    cg.add(var.set_zero_pin(zero_pin))
    one_pin = await cg.gpio_pin_expression(config["one_pin"])
    cg.add(var.set_one_pin(one_pin))
    two_pin = await cg.gpio_pin_expression(config["two_pin"])
    cg.add(var.set_two_pin(two_pin))
    three_pin = await cg.gpio_pin_expression(config["three_pin"])
    cg.add(var.set_three_pin(three_pin))
    cg.add(var.set_count_back(config["count_back"]))
    cg.add(var.set_count_back_speed(config["count_back_speed"]))
