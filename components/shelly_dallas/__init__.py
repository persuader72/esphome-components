import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_ID, CONF_PIN_A, CONF_PIN_B

MULTI_CONF = True
AUTO_LOAD = ['sensor']

CONF_ONE_WIRE_ID = 'one_wire_id'
shelly_dallas_ns = cg.esphome_ns.namespace('shelly_dallas')
ShellyDallasComponent = shelly_dallas_ns.class_('ShellyDallasComponent', cg.PollingComponent)
ESPOneWire = shelly_dallas_ns.class_('ESPOneWire')

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ShellyDallasComponent),
    cv.GenerateID(CONF_ONE_WIRE_ID): cv.declare_id(ESPOneWire),
    cv.Required(CONF_PIN_A): pins.gpio_input_pin_schema,
    cv.Required(CONF_PIN_B): pins.gpio_output_pin_schema,
}).extend(cv.polling_component_schema('60s'))


def to_code(config):
    in_pin = yield cg.gpio_pin_expression(config[CONF_PIN_A])
    out_pin = yield cg.gpio_pin_expression(config[CONF_PIN_B])
    one_wire = cg.new_Pvariable(config[CONF_ONE_WIRE_ID], in_pin, out_pin)
    var = cg.new_Pvariable(config[CONF_ID], one_wire)
    yield cg.register_component(var, config)
