from esphome.components import light
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_OUTPUT_ID, CONF_MIN_VALUE, CONF_MAX_VALUE, CONF_GAMMA_CORRECT, \
    CONF_DEFAULT_TRANSITION_LENGTH, CONF_RESTORE_MODE
from .. import CONF_DALI_ID, DALIComponent, dali_ns, dali_device_schema, register_dali_device

DEPENDENCIES = ['dali']

DALILight = dali_ns.class_('DALILight', light.LightOutput, cg.Component)

CONFIG_SCHEMA = light.BRIGHTNESS_ONLY_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(DALILight),
    cv.GenerateID(CONF_DALI_ID): cv.use_id(DALIComponent),
    cv.Optional(CONF_MIN_VALUE): cv.int_,
    cv.Optional(CONF_MAX_VALUE): cv.int_,
}).extend(cv.COMPONENT_SCHEMA).extend(dali_device_schema(0x20))


def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    yield light.register_light(var, config)

    if CONF_MIN_VALUE in config:
        cg.add(var.set_min_value(config[CONF_MIN_VALUE]))
    if CONF_MAX_VALUE in config:
        cg.add(var.set_max_value(config[CONF_MAX_VALUE]))
    # paren = yield cg.get_variable(config[CONF_DALI_ID])
    # cg.add(var.set_dali_parent(paren))
    yield register_dali_device(var, config)
