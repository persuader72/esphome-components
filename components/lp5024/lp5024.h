#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace lp5024 {

struct LPRGB { uint8_t r; uint8_t g; uint8_t b; };

class LP5024LightOutputBase : public light::AddressableLight, public i2c::I2CDevice {
public:
  enum ErrorCode { NONE = 0, COMMUNICATION_FAILED, WRONG_ID, } mErrorCode{NONE};
private:
     static unsigned long elapsedMillis(unsigned long t2, unsigned long t1) { return t2 >= t1 ? t2 - t1 : (~(t1 - t2)) + 1; }
     uint32_t mLastTime = 0;
public:
  void setup() override;
  void dump_config() override;
  //float get_setup_priority() const override;
  void loop() override;
public:
  int32_t size() const override;
  void clear_effect_data() override;
  light::LightTraits get_traits();
private:
  light::ESPColorView get_view_internal(int32_t index) const override {
      return {(uint8_t *)&this->mLeds[index].r, (uint8_t *)&this->mLeds[index].g, (uint8_t *)&this->mLeds[index].b, nullptr, nullptr, &this->correction_};
  }
  LPRGB mLeds[16];
};

}  // namespace apds9960
}  // namespace esphome
