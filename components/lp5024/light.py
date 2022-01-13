import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.components import light
from esphome.const import CONF_OUTPUT_ID

DEPENDENCIES = ['i2c']

lp5024_ns = cg.esphome_ns.namespace('lp5024')
LP5024LightOutputBase = lp5024_ns.class_('LP5024LightOutputBase', light.AddressableLight)


CONFIG_SCHEMA = cv.All(light.ADDRESSABLE_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(LP5024LightOutputBase),
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x50)))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)
    yield light.register_light(var, config)
