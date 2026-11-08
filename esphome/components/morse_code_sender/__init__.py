from esphome import automation
import esphome.codegen as cg
from esphome.components import switch
from esphome.components.const import CONF_KEYS
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.types import ConfigType

CODEOWNERS = ["@lylavoie"]
DEPENDENCIES = ["switch"]
DOMAIN = "morse_code_sender"

CONF_SWITCH_ID = "switch_id"
CONF_WPM = "wpm"

morse_code_sender_ns = cg.esphome_ns.namespace("morse_code_sender")
MorseCodeSender = morse_code_sender_ns.class_("MorseCodeSender", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(MorseCodeSender),
        cv.Required(CONF_SWITCH_ID): cv.use_id(switch.Switch),
        cv.Optional(CONF_WPM, default=20): cv.int_range(min=1, max=100),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config: ConfigType) -> None:
    sw = await cg.get_variable(config[CONF_SWITCH_ID])
    var = cg.new_Pvariable(config[CONF_ID], sw)
    await cg.register_component(var, config)
    cg.add(var.set_wpm(config[CONF_WPM]))


def validate_keys(value: str) -> str:
    value = cv.string_strict(value)
    if invalid := set(value) - {".", "-", " "}:
        raise cv.Invalid(
            f"Invalid characters {sorted(invalid)}; only '.', '-' and ' ' are allowed"
        )
    return value


SEND_KEYS_ACTION_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(MorseCodeSender),
        cv.Required(CONF_KEYS): cv.templatable(validate_keys),
    }
)

automation.register_apply_action(
    "morse_code_sender.send_keys",
    SEND_KEYS_ACTION_SCHEMA,
    automation.ApplyField(CONF_KEYS, "send_keys", cg.std_string),
)
