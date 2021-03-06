import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

AUTO_LOAD = ['climate_ir']

mitsubishi_general_ns = cg.esphome_ns.namespace('mitsubishi_general')
MitsubishiGeneralClimate = mitsubishi_general_ns.class_('MitsubishiGeneralClimate', climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.CLIMATE_IR_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(MitsubishiGeneralClimate),
})


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield climate_ir.register_climate_ir(var, config)
