import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ADDRESS, CONF_DALLAS_ID, CONF_INDEX, CONF_RESOLUTION, UNIT_CELSIUS, \
    ICON_THERMOMETER, CONF_ID
from . import ShellyDallasComponent, shelly_dallas_ns

DallasTemperatureSensor = shelly_dallas_ns.class_('ShellyDallasTemperatureSensor', sensor.Sensor)

CONFIG_SCHEMA = cv.All(sensor.sensor_schema(UNIT_CELSIUS).extend({
    cv.GenerateID(): cv.declare_id(DallasTemperatureSensor),
    cv.GenerateID(CONF_DALLAS_ID): cv.use_id(ShellyDallasComponent),

    cv.Optional(CONF_ADDRESS): cv.hex_int,
    cv.Optional(CONF_INDEX): cv.positive_int,
    cv.Optional(CONF_RESOLUTION, default=12): cv.int_range(min=9, max=12),
}), cv.has_exactly_one_key(CONF_ADDRESS, CONF_INDEX))


def to_code(config):
    hub = yield cg.get_variable(config[CONF_DALLAS_ID])
    if CONF_ADDRESS in config:
        address = config[CONF_ADDRESS]
        rhs = hub.Pget_sensor_by_address(address, config.get(CONF_RESOLUTION))
    else:
        rhs = hub.Pget_sensor_by_index(config[CONF_INDEX], config.get(CONF_RESOLUTION))
    var = cg.Pvariable(config[CONF_ID], rhs)
    yield sensor.register_sensor(var, config)
