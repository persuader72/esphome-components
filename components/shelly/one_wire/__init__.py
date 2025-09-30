from esphome import pins
import esphome.codegen as cg
from esphome.components.one_wire import OneWireBus
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_INPUT, CONF_OUTPUT

from .. import shelly_ns

CODEOWNERS = ["@persuader72"]

ShellyOneWireBus = shelly_ns.class_("ShellyOneWireBus", OneWireBus, cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ShellyOneWireBus),
        cv.Required(CONF_INPUT): pins.internal_gpio_input_pin_schema,
        cv.Required(CONF_OUTPUT): pins.internal_gpio_output_pin_schema,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    pin_i = await cg.gpio_pin_expression(config[CONF_INPUT])
    pin_o = await cg.gpio_pin_expression(config[CONF_OUTPUT])
    cg.add(var.set_pins(pin_i, pin_o))
