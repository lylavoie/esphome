import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import spi
from esphome.const import (
    CONF_ID,
    CONF_SPI_ID,
    CONF_NUMBER,
    CONF_INVERTED,
    CONF_DATA_PIN,
    CONF_CLOCK_PIN,
    CONF_OUTPUT,
    CONF_COUNT,
)
from esphome.core import EsphomeError

MULTI_CONF = True

CODEOWNERS = ["@lylavoie"]

HV5522_ns = cg.esphome_ns.namespace("hv5522")
HV5522component = HV5522_ns.class_("HV5522component", cg.Component)
HV5522GPIOcomponent = HV5522_ns.class_("HV5522GPIOcomponent", HV5522component)
HV5522SPIcomponent = HV5522_ns.class_(
    "HV5522SPIcomponent", HV5522component, spi.SPIDevice
)
HV5522Pin = HV5522_ns.class_(
    "HV5522Pin", cg.GPIOPin, cg.Parented.template(HV5522component)
)

CONF_HV5522 = "hv5522"
CONF_LATCH_PIN = "latch_pin"

CONFIG_SCHEMA = cv.Any(
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(HV5522GPIOcomponent),
            cv.Required(CONF_DATA_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_CLOCK_PIN): pins.gpio_output_pin_schema,
            cv.Required(CONF_LATCH_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_COUNT, default=1): cv.int_range(min=1, max=4),
        }
    ).extend(cv.COMPONENT_SCHEMA),
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.declare_id(HV5522SPIcomponent),
            cv.Required(CONF_LATCH_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_COUNT, default=1): cv.int_range(min=1, max=4),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(spi.spi_device_schema(cs_pin_required=False))
    .extend(
        {
            cv.Required(CONF_SPI_ID): cv.use_id(spi.SPIComponent),
        }
    ),
    msg='Either "data_pin" and "clock_pin" must be set or "spi_id" must be set.',
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_DATA_PIN in config:
        data_pin = await cg.gpio_pin_expression(config[CONF_DATA_PIN])
        cg.add(var.set_data_pin(data_pin))
        clock_pin = await cg.gpio_pin_expression(config[CONF_CLOCK_PIN])
        cg.add(var.set_clock_pin(clock_pin))
    elif CONF_SPI_ID in config:
        await spi.register_spi_device(var, config)
    else:
        raise EsphomeError("Not supported")
    latch_pin = await cg.gpio_pin_expression(config[CONF_LATCH_PIN])
    cg.add(var.set_latch_pin(latch_pin))
    cg.add(var.set_chip_count(config[CONF_COUNT]))


def _validate_output_mode(value):
    if value.get(CONF_OUTPUT) is not True:
        raise cv.Invalid("Only output mode is supported")
    return value


HV5522_PIN_SCHEMA = pins.gpio_base_schema(
    HV5522Pin,
    cv.int_range(min=0, max=128),
    modes=[CONF_OUTPUT],
    mode_validator=_validate_output_mode,
    invertable=True,
).extend(
    {
        cv.Required(CONF_HV5522): cv.use_id(HV5522component),
    }
)


def hv5522_pin_final_validate(pin_config, parent_config):
    max_pins = parent_config[CONF_COUNT] * 32
    if pin_config[CONF_NUMBER] >= max_pins:
        raise cv.Invalid(f"Pin number must be less than {max_pins}")


@pins.PIN_SCHEMA_REGISTRY.register(
    CONF_HV5522, HV5522_PIN_SCHEMA, hv5522_pin_final_validate
)
async def hv5522_pin_to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_parented(var, config[CONF_HV5522])
    cg.add(var.set_pin(config[CONF_NUMBER]))
    cg.add(var.set_inverted(config[CONF_INVERTED]))
    return var
