import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output, i2c
from esphome.const import CONF_ID, CONF_ADDRESS, CONF_INITIAL_VALUE

CODEOWNERS = ["@lylavoie"]
DEPENDENCIES = ["i2c"]

mcp401x_ns = cg.esphome_ns.namespace("mcp401x")
MCP401x = mcp401x_ns.class_("MCP401x", output.FloatOutput, cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.All(
    output.FLOAT_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(CONF_ID): cv.declare_id(MCP401x),
            cv.Optional(CONF_INITIAL_VALUE, default=0.5): cv.float_range(
                min=0, max=1.0
            ),
        }
    ).extend(i2c.i2c_device_schema(0x2F)),
)


async def to_code(config):
    # Chip is not addressable, override to the single value
    config[CONF_ADDRESS] = 0x2F
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    await output.register_output(var, config)
    cg.add(var.set_initial_value(config[CONF_INITIAL_VALUE]))
