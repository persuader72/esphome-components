#include "mitsubishi_general.h"
#include <esphome/core/log.h>

namespace esphome {
namespace mitsubishi_general {

static const char *TAG = "mitsubishi_general.climate";

// Control packet
const uint16_t MITSUBISHI_STATE_LENGTH = 14;
const uint8_t MITSUBISHI_DEFAULT_STATE[MITSUBISHI_STATE_LENGTH] = {0x23, 0xCB, 0x26, 0x01, 0x00, 0x24, 0x03, 0x0B, 0x10, 0x00, 0x00, 0x00, 0x00, 0x30};
const uint8_t MITSUBISHI_CRC_BYTE = 13;

// Temperature and POWER ON
const uint8_t MITSUBISHI_POWER_MASK  = 0b00000100;
const uint8_t MITSUBISHI_POWER_ON    = 0b00000100;
const uint8_t MITSUBISHI_POWER_OFF   = 0b00000000;

// Mode (default AUTO)
const uint8_t MITSUBISHI_MODE_MASK   = 0b00000111;
const uint8_t MITSUBISHI_MODE_AUTO   = 0b00000111;
const uint8_t MITSUBISHI_MODE_HEAT   = 0b00000001;
const uint8_t MITSUBISHI_MODE_COOL   = 0b00000011;
const uint8_t MITSUBISHI_MODE_DRY    = 0b00000010;

const uint8_t MITSUBISHI_TEMP_MASK   = 0b00001111;

const uint8_t MITSUBISHI_FAN_MASK    = 0b10000111;// Fan speed and swing
const uint8_t MITSUBISHI_FAN_AUTO    = 0b10000000;
const uint8_t MITSUBISHI_FAN_HIGH    = 0b00000100;
const uint8_t MITSUBISHI_FAN_MEDIUM  = 0b00000010;
const uint8_t MITSUBISHI_FAN_LOW     = 0b00000001;
const uint8_t MITSUBISHI_FAN_SILENT  = 0b00000101;

const uint8_t MITSUBISHI_VANNE_MASK    = 0b00111000;
const uint8_t MITSUBISHI_VANNE_AUTO    = 0b00111000;
const uint8_t MITSUBISHI_VANNE_LOWEST  = 0b00001000;
const uint8_t MITSUBISHI_VANNE_LOW     = 0b00100000;
const uint8_t MITSUBISHI_VANNE_MIDDLE  = 0b00011000;
const uint8_t MITSUBISHI_VANNE_HIGH    = 0b00010000;
const uint8_t MITSUBISHI_VANNE_HIGHEST = 0b00001000;
const uint8_t MITSUBISHI_VANNE_MOVE    = 0b00000000;

const uint8_t MITSUBISHI_TEMP_MAX = 31;  // Celsius
const uint8_t MITSUBISHI_TEMP_MIN = 16;  // Celsius

const uint16_t MITSUBISHI_HEADER_MARK = 3400;
const uint16_t MITSUBISHI_HEADER_SPACE = 1750;
const uint16_t MITSUBISHI_BIT_MARK = 450;
const uint16_t MITSUBISHI_ONE_SPACE = 1300;
const uint16_t MITSUBISHI_ZERO_SPACE = 420;
const uint16_t MITSUBISHI_TRL_MARK = 440;
const uint16_t MITSUBISHI_TRL_SPACE = 17100;

const uint32_t MITSUBISHI_CARRIER_FREQUENCY = 38000;

MitsubishiGeneralClimate::MitsubishiGeneralClimate() : ClimateIR(MITSUBISHI_TEMP_MIN, MITSUBISHI_TEMP_MAX, 1) {}
  
bool MitsubishiGeneralClimate::on_receive(remote_base::RemoteReceiveData data) {
  return false;
}

void MitsubishiGeneralClimate::control(const climate::ClimateCall &call) {
  ClimateIR::control(call);

  if (call.get_fan_mode().has_value()) {
        ESP_LOGD("control", "Requested fan mode is %s", call.get_fan_mode());
        this->fan_mode = *call.get_fan_mode();
        /*switch(*call.get_fan_mode()) {
            case climate::CLIMATE_FAN_DIFFUSE:
              mFanSpeed = MITSUBISHI_FAN_SILENT_BYTE9;
              break;
            case climate::CLIMATE_FAN_LOW:
              mFanSpeed = MITSUBISHI_FAN_LOW_BYTE9;
              break;
            case climate::CLIMATE_FAN_MEDIUM:
              mFanSpeed = MITSUBISHI_FAN_MEDIUM_BYTE9;
              break;
            case climate::CLIMATE_FAN_MIDDLE:
              mFanSpeed = MITSUBISHI_FAN_MEDIUM_BYTE9;
              break;
            case climate::CLIMATE_FAN_HIGH:
              mFanSpeed = MITSUBISHI_FAN_HIGH_BYTE9;
              break;
            default:
              mFanSpeed = MITSUBISHI_FAN_AUTO_BYTE9;
              break;
        }*/
    }

    if(call.get_swing_mode().has_value()) {
      ESP_LOGD("control", "Requested swing mode is %s", call.get_swing_mode());
      switch(*call.get_fan_mode()) {
      case climate::CLIMATE_SWING_OFF:
        mSwingMode = MITSUBISHI_VANNE_MOVE;
        break;
      default:
        mSwingMode = MITSUBISHI_VANNE_MOVE;
        break;
      }
    }
}

climate::ClimateTraits MitsubishiGeneralClimate::traits() {
  auto traits = ClimateIR::traits();
  traits.set_supports_action(true);
  //traits.set_supports_current_temperature(this->sensor_ != nullptr);
  //traits.set_supports_auto_mode(true);
  //traits.set_supports_cool_mode(this->supports_cool_);
  //traits.set_supports_heat_mode(this->supports_heat_);
  traits.set_supports_dry_mode(true);
  traits.set_supports_fan_only_mode(true);
  //traits.set_supports_two_point_target_temperature(false);
  //traits.set_supports_away(false);
  //traits.set_visual_min_temperature(this->minimum_temperature_);
  //traits.set_visual_max_temperature(this->maximum_temperature_);
  //traits.set_visual_temperature_step(this->temperature_step_);
  traits.set_supports_fan_mode_on(false);
  traits.set_supports_fan_mode_off(false);
  traits.set_supports_fan_mode_auto(true);
  traits.set_supports_fan_mode_focus(false);
  traits.set_supports_fan_mode_diffuse(true);
  traits.set_supports_fan_mode_low(true);
  traits.set_supports_fan_mode_medium(true);
  traits.set_supports_fan_mode_middle(true);
  traits.set_supports_fan_mode_high(true);
  traits.set_supports_swing_mode_off(true);
  traits.set_supports_swing_mode_both(false);
  traits.set_supports_swing_mode_vertical(true);
  traits.set_supports_swing_mode_horizontal(false);
  return traits;
}

void MitsubishiGeneralClimate::transmit_state() {
  
  uint8_t remote_state[MITSUBISHI_STATE_LENGTH];
  for(int i=0;i<MITSUBISHI_STATE_LENGTH;i++) remote_state[i] = MITSUBISHI_DEFAULT_STATE[i];

  // If not powered - set power on flag
  remote_state[5] &= ~MITSUBISHI_POWER_MASK;
  if (!this->power_ || mode==climate::CLIMATE_MODE_OFF) {
    remote_state[5] |= MITSUBISHI_POWER_OFF;
  } else {
    remote_state[5] |= MITSUBISHI_POWER_ON;
  }

  // Set mode
  remote_state[6] &= ~MITSUBISHI_MODE_MASK;
  switch (this->mode) {
    case climate::CLIMATE_MODE_OFF:
      remote_state[6] |= MITSUBISHI_MODE_AUTO;
    case climate::CLIMATE_MODE_COOL:
      remote_state[6] |= MITSUBISHI_MODE_COOL;
      break;
    case climate::CLIMATE_MODE_HEAT:
      remote_state[6] |= MITSUBISHI_MODE_HEAT;
      break;
    case climate::CLIMATE_MODE_DRY:
      remote_state[6] |= MITSUBISHI_MODE_DRY;
      break;
    case climate::CLIMATE_MODE_AUTO:
    default:
      remote_state[6] |= MITSUBISHI_MODE_AUTO;
      break;
  }

    // Set temperature
  uint8_t safecelsius = std::max((uint8_t) this->target_temperature, MITSUBISHI_TEMP_MIN);
  safecelsius = std::min(safecelsius, MITSUBISHI_TEMP_MAX);
  remote_state[7] &= ~MITSUBISHI_TEMP_MASK;
  remote_state[7] |= (uint8_t)(safecelsius - MITSUBISHI_TEMP_MIN);

  remote_state[8] &= ~MITSUBISHI_FAN_MASK;
  remote_state[8] |= mFanSpeed;

  remote_state[8] &= ~MITSUBISHI_VANNE_MASK;
  remote_state[8] |= MITSUBISHI_VANNE_MOVE;

  // CRC
  remote_state[MITSUBISHI_CRC_BYTE] = 0;
  for (int i = 0; i < MITSUBISHI_CRC_BYTE-1; i++) {
    remote_state[13] += (byte) remote_state[i];  // Addiction
  }

  auto transmit = this->transmitter_->transmit();
  auto data = transmit.get_data();

  data->set_carrier_frequency(MITSUBISHI_CARRIER_FREQUENCY);

  // Header
  data->mark(MITSUBISHI_HEADER_MARK);
  data->space(MITSUBISHI_HEADER_SPACE);

  // Data
  for (uint8_t i : remote_state) {
    // Send all Bits from Byte Data in Reverse Order
    for (uint8_t mask = 00000001; mask > 0; mask <<= 1) {  // iterate through bit mask
      data->mark(MITSUBISHI_BIT_MARK);
      bool bit = i & mask;
      data->space(bit ? MITSUBISHI_ONE_SPACE : MITSUBISHI_ZERO_SPACE);
      // Next bits
    }
  }

  // Footer
  data->mark(MITSUBISHI_TRL_MARK);
  data->space(MITSUBISHI_TRL_SPACE);

  transmit.perform();

  this->power_ = true;
}

}  // namespace mitsubishi_general
}  // namespace esphome
