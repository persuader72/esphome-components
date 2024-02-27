#include "hisense_ac.h"

namespace esphome {
namespace hisense_ac {

static const char *TAG = "mitsubishi_general.climate";

// Control packet
const uint16_t MITSUBISHI_GENERAL_STATE_LENGTH = 14;

const uint8_t MITSUBISHI_GENERAL_BASE_BYTE0 = 0x23;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE1 = 0xCB;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE2 = 0x26;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE3 = 0x01;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE4 = 0x00;

// Temperature and POWER ON
const uint8_t MITSUBISHI_GENERAL_POWER_ON_MASK_BYTE5 = 0b00000100;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE5 = 0x24;

// Mode (default AUTO)
const uint8_t MITSUBISHI_GENERAL_MODE_AUTO_BYTE6 = 0x08;
const uint8_t MITSUBISHI_GENERAL_MODE_HEAT_BYTE6 = 0x01;
const uint8_t MITSUBISHI_GENERAL_MODE_COOL_BYTE6 = 0x03;
const uint8_t MITSUBISHI_GENERAL_MODE_DRY_BYTE6 = 0x02;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE6 = 0x08;

// Temperaute (default AUTO)
const uint8_t MITSUBISHI_GENERAL_TEMP_AUTO_BYTE7 = 0x06;
const uint8_t MITSUBISHI_GENERAL_TEMP_HEAT_BYTE7 = 0x05;
const uint8_t MITSUBISHI_GENERAL_TEMP_COOL_BYTE7 = 0x0b;
const uint8_t MITSUBISHI_GENERAL_TEMP_DRY_BYTE7 = 0x07;
//const uint8_t MITSUBISHI_GENERAL_MODE_FAN_BYTE7 = 0x03;
//const uint8_t MITSUBISHI_GENERAL_MODE_10C_BYTE7 = 0x0B;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE7 = 0x06;

// Fan speed and swing
const uint8_t MITSUBISHI_GENERAL_FAN_AUTO_BYTE8 = 0x00;
const uint8_t MITSUBISHI_GENERAL_FAN_HIGH_BYTE8 = 0x01;
const uint8_t MITSUBISHI_GENERAL_FAN_MEDIUM_BYTE8 = 0x02;
const uint8_t MITSUBISHI_GENERAL_FAN_LOW_BYTE8 = 0x03;
const uint8_t MITSUBISHI_GENERAL_FAN_SILENT_BYTE8 = 0x04;
const uint8_t MITSUBISHI_GENERAL_SWING_MASK_BYTE8 = 0b00010000;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE8 = 0x0A;

const uint8_t MITSUBISHI_GENERAL_BASE_BYTE9 = 0x01;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE10 = 0x00;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE11 = 0x00;
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE12 = 0x00;
// CRC
const uint8_t MITSUBISHI_GENERAL_BASE_BYTE13 = 0x51;

// Outdoor Unit Low Noise
const uint8_t MITSUBISHI_GENERAL_OUTDOOR_UNIT_LOW_NOISE_BYTE14 = 0xA0;

const uint8_t MITSUBISHI_GENERAL_TEMP_MAX = 31;  // Celsius
const uint8_t MITSUBISHI_GENERAL_TEMP_MIN = 16;  // Celsius

const uint16_t MITSUBISHI_GENERAL_HEADER_MARK = 9075;
const uint16_t MITSUBISHI_GENERAL_HEADER_SPACE = 4445;
const uint16_t MITSUBISHI_GENERAL_BIT_MARK = 500;
const uint16_t MITSUBISHI_GENERAL_ONE_SPACE = 1750;
const uint16_t MITSUBISHI_GENERAL_ZERO_SPACE = 606;
const uint16_t MITSUBISHI_GENERAL_TRL_MARK = 440;
const uint16_t MITSUBISHI_GENERAL_TRL_SPACE = 17100;

const uint32_t MITSUBISHI_GENERAL_CARRIER_FREQUENCY = 38000;

HisenseAcClimate::HisenseAcClimate() : ClimateIR(MITSUBISHI_GENERAL_TEMP_MIN, MITSUBISHI_GENERAL_TEMP_MAX, 1) {}

bool HisenseAcClimate::on_receive(remote_base::RemoteReceiveData data) {
  return false;
}

void HisenseAcClimate::transmit_state() {
  uint8_t remote_state[MITSUBISHI_GENERAL_STATE_LENGTH] = {0};

  remote_state[0] = MITSUBISHI_GENERAL_BASE_BYTE0;
  remote_state[1] = MITSUBISHI_GENERAL_BASE_BYTE1;
  remote_state[2] = MITSUBISHI_GENERAL_BASE_BYTE2;
  remote_state[3] = MITSUBISHI_GENERAL_BASE_BYTE3;
  remote_state[4] = MITSUBISHI_GENERAL_BASE_BYTE4;
  remote_state[5] = MITSUBISHI_GENERAL_BASE_BYTE5;
  remote_state[6] = MITSUBISHI_GENERAL_BASE_BYTE6;
  remote_state[7] = MITSUBISHI_GENERAL_BASE_BYTE7;
  remote_state[8] = MITSUBISHI_GENERAL_BASE_BYTE8;
  remote_state[9] = MITSUBISHI_GENERAL_BASE_BYTE9;
  remote_state[10] = MITSUBISHI_GENERAL_BASE_BYTE10;
  remote_state[11] = MITSUBISHI_GENERAL_BASE_BYTE11;
  remote_state[12] = MITSUBISHI_GENERAL_BASE_BYTE12;
  remote_state[13] = MITSUBISHI_GENERAL_BASE_BYTE13;

  // Set temperature
  uint8_t safecelsius = std::max((uint8_t) this->target_temperature, MITSUBISHI_GENERAL_TEMP_MIN);
  safecelsius = std::min(safecelsius, MITSUBISHI_GENERAL_TEMP_MAX);
  remote_state[7] = (byte) safecelsius - MITSUBISHI_GENERAL_TEMP_MIN;

  // If not powered - set power on flag
  if (!this->power_ || mode==climate::CLIMATE_MODE_OFF) {
    remote_state[5] = (byte) remote_state[5] | MITSUBISHI_GENERAL_POWER_ON_MASK_BYTE5;
  }

  // Set mode
  switch (this->mode) {
    case climate::CLIMATE_MODE_OFF:
      remote_state[6] = MITSUBISHI_GENERAL_MODE_AUTO_BYTE6;
      remote_state[5] &= ~MITSUBISHI_GENERAL_POWER_ON_MASK_BYTE5;
    case climate::CLIMATE_MODE_COOL:
      remote_state[6] = MITSUBISHI_GENERAL_MODE_COOL_BYTE6;
      break;
    case climate::CLIMATE_MODE_HEAT:
      remote_state[6] = MITSUBISHI_GENERAL_MODE_HEAT_BYTE6;
      break;
    case climate::CLIMATE_MODE_AUTO:
    default:
      remote_state[6] = MITSUBISHI_GENERAL_MODE_AUTO_BYTE6;
      break;
      // TODO: CLIMATE_MODE_FAN_ONLY, CLIMATE_MODE_DRY, CLIMATE_MODE_10C are missing in esphome
  }

  // TODO: missing support for fan speed
  // remote_state[10] = MITSUBISHI_GENERAL_FAN_AUTO_BYTE10;

  // TODO: missing support for swing
  // remote_state[10] = (byte) remote_state[10] | MITSUBISHI_GENERAL_SWING_MASK_BYTE10;

  // TODO: missing support for outdoor unit low noise
  // remote_state[14] = (byte) remote_state[14] | MITSUBISHI_GENERAL_OUTDOOR_UNIT_LOW_NOISE_BYTE14;

  // CRC
  remote_state[13] = 0;
  for (int i = 0; i < MITSUBISHI_GENERAL_STATE_LENGTH-1; i++) {
    remote_state[13] += (byte) remote_state[i];  // Addiction
  }

  auto transmit = this->transmitter_->transmit();
  auto data = transmit.get_data();

  data->set_carrier_frequency(MITSUBISHI_GENERAL_CARRIER_FREQUENCY);

  // Header
  data->mark(MITSUBISHI_GENERAL_HEADER_MARK);
  data->space(MITSUBISHI_GENERAL_HEADER_SPACE);
  // Data
  for (uint8_t i : remote_state) {
    // Send all Bits from Byte Data in Reverse Order
    for (uint8_t mask = 00000001; mask > 0; mask <<= 1) {  // iterate through bit mask
      data->mark(MITSUBISHI_GENERAL_BIT_MARK);
      bool bit = i & mask;
      data->space(bit ? MITSUBISHI_GENERAL_ONE_SPACE : MITSUBISHI_GENERAL_ZERO_SPACE);
      // Next bits
    }
  }
  // Footer
  data->mark(MITSUBISHI_GENERAL_TRL_MARK);
  data->space(MITSUBISHI_GENERAL_TRL_SPACE);

  transmit.perform();

  this->power_ = true;
}

}  // namespace mitsubishi_general
}  // namespace esphome
