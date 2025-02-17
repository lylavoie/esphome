from esphome.codegen import uint32
from esphome.components.lvgl.helpers import lvgl_components_required
from esphome.components.lvgl.lvcode import lv, lv_add, lv_expr
import esphome.config_validation as cv
from esphome.const import CONF_COLOR, CONF_HEIGHT, CONF_WIDTH

from ..defines import CONF_ANIMATED, CONF_KNOB, CONF_MAIN
from ..lv_validation import animated, lv_color, size
from ..types import LvType, WidgetType

# The return value for the colorwheel is defined as an unsigned 32 bit integer, so it can
# store a 24-bit color.  This ensures its behavior is the same as setting the color via
# lv_color.process(color). Interally, LVGL and the colorwheel widget use the 16-bit color
# depth.
lv_colorwheel_t = LvType(
    "lv_colorwheel_t",
    largs=[(uint32, "x")],
    lvalue=lambda w: lv_add(
        lv_expr.call("color_to32", lv_expr.call("colorwheel_get_rgb", w.obj)) & 0xFFFFFF
    ),
    has_on_value=True,
    value_property=CONF_COLOR,
)


CONF_COLORWHEEL = "colorwheel"
CONF_KNOB_RECOLOR = "knob_recolor"
CONF_FIXED_MODE = "fixed_mode"
COLORWHEEL_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_WIDTH): size,
        cv.Optional(CONF_HEIGHT): cv.invalid("Height will be set to the same as width"),
        cv.Optional(CONF_KNOB_RECOLOR, default=True): cv.boolean,
        cv.Optional(CONF_FIXED_MODE, default=False): cv.boolean,
        cv.Optional(CONF_COLOR): lv_color,
        cv.Optional(CONF_ANIMATED, default=True): animated,
    }
)
COLORWHEEL_MODIFY_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_COLOR): lv_color,
        cv.Optional(CONF_ANIMATED, default=True): animated,
    }
)


class ColorWheelType(WidgetType):
    def __init__(self):
        super().__init__(
            name=CONF_COLORWHEEL,
            w_type=lv_colorwheel_t,
            lv_name="colorwheel",
            parts=(CONF_MAIN, CONF_KNOB),
            schema=COLORWHEEL_SCHEMA,
            modify_schema=COLORWHEEL_MODIFY_SCHEMA,
        )

    @property
    def animated(self):
        return True

    def get_uses(self):
        return "arc"

    async def to_code(self, w, config):
        lvgl_components_required.add(CONF_COLORWHEEL)
        if color := config.get(CONF_COLOR):
            lv.colorwheel_set_rgb(w.obj, await lv_color.process(color))
        if mode := config.get(CONF_FIXED_MODE):
            lv.colorwheel_set_mode_fixed(w.obj, mode)
        if CONF_WIDTH in config:
            w.set_style(CONF_HEIGHT, await size.process(config[CONF_WIDTH]), 0)

    def obj_creator(self, parent, config):
        knob_recolor = config[CONF_KNOB_RECOLOR]
        return lv_expr.call("colorwheel_create", parent, knob_recolor)


colorwheel_spec = ColorWheelType()
