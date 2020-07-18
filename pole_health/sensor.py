import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import ICON_ROTATE_RIGHT, CONF_ID, UNIT_DEGREES, CONF_ROTATION

polehealth_ns = cg.esphome_ns.namespace('polehealth')
PoleHealthSensor = polehealth_ns.class_('PoleHealthSensor', cg.Component, sensor.Sensor)

rotation_schema = sensor.sensor_schema(UNIT_DEGREES, ICON_ROTATE_RIGHT, 1)

CONF_ACCEL_X = "in_accel_x"
CONF_ACCEL_Y = "in_accel_y"
CONF_ACCEL_Z = "in_accel_z"

CONFIG_SCHEMA = sensor.sensor_schema(UNIT_DEGREES, ICON_ROTATE_RIGHT, 1).extend({
    cv.GenerateID(): cv.declare_id(PoleHealthSensor),
    cv.Required(CONF_ACCEL_X): cv.use_id(sensor.Sensor),
    cv.Required(CONF_ACCEL_Y): cv.use_id(sensor.Sensor),
    cv.Required(CONF_ACCEL_Z): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_ROTATION): rotation_schema,
}).extend(cv.COMPONENT_SCHEMA)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)

    sens_x = yield cg.get_variable(config[CONF_ACCEL_X])
    cg.add(var.set_accel_x(sens_x))
    sens_y = yield cg.get_variable(config[CONF_ACCEL_Y])
    cg.add(var.set_accel_y(sens_y))
    sens_z = yield cg.get_variable(config[CONF_ACCEL_Z])
    cg.add(var.set_accel_z(sens_z))

    if CONF_ROTATION in config:
        sens = yield sensor.new_sensor(config[CONF_ROTATION])
        cg.add(var.set_roation_sensor(sens))
