import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import CONF_FREQUENCY, CONF_ID, CONF_TX_PIN, CONF_RX_PIN, CONF_ADDRESS
from esphome.core import coroutine, coroutine_with_priority

dali_ns = cg.esphome_ns.namespace('dali')
DALIComponent = dali_ns.class_('DALIComponent', cg.Component)
DALIDevice = dali_ns.class_('DALIDevice')
CONF_DALI_ID = 'dali_id'

MULTI_CONF = True
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DALIComponent),
    cv.Required(CONF_TX_PIN): pins.gpio_output_pin_schema,
    cv.Required(CONF_RX_PIN): pins.gpio_input_pin_schema,
    cv.Optional(CONF_FREQUENCY, default='2403'):
    cv.All(cv.frequency, cv.Range(min=0, min_included=False)),
}).extend(cv.COMPONENT_SCHEMA)


@coroutine_with_priority(1.0)
def to_code(config):
    cg.add_define('USE_DALI')
    cg.add_global(dali_ns.using)
    tx_pin = yield cg.gpio_pin_expression(config[CONF_TX_PIN])
    rx_pin = yield cg.gpio_pin_expression(config[CONF_RX_PIN])
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)

    cg.add(var.set_tx_pin(tx_pin))
    cg.add(var.set_rx_pin(rx_pin))
    cg.add(var.set_frequency(int(config[CONF_FREQUENCY])))


def dali_device_schema(default_address):
    """Create a schema for a i2c device.

    :param default_address: The default address of the i2c device, can be None to represent
      a required option.
    :return: The i2c device schema, `extend` this in your config schema.
    """
    schema = {
        cv.GenerateID(CONF_DALI_ID): cv.use_id(DALIComponent),
    }
    if default_address is None:
        schema[cv.Required(CONF_ADDRESS)] = cv.int_
    else:
        schema[cv.Optional(CONF_ADDRESS, default=default_address)] = cv.int_
    return cv.Schema(schema)


@coroutine
def register_dali_device(var, config):
    """Register an i2c device with the given config.

    Sets the i2c bus to use and the i2c address.

    This is a coroutine, you need to await it with a 'yield' expression!
    """
    parent = yield cg.get_variable(config[CONF_DALI_ID])
    cg.add(parent.register_device(var))
    cg.add(var.set_dali_parent(parent))
    cg.add(var.set_dali_address(config[CONF_ADDRESS]))
