#include "dali_light.h"

namespace esphome {
namespace dali {

static const char *TAG_LIGHT = "dali::light";

void DALILight::loop() {
  uint32_t now = millis();
  if(elapsedMillis(now, mLastPowerRefresh) > 2000) {
    mLastPowerRefresh = now;
    set_power(mPower);
  }
}

light::LightTraits DALILight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supports_brightness(true);
  return traits;
}

void DALILight::write_state(light::LightState *state) {
    float brightness;
    state->current_values_as_brightness(&brightness);
    uint8_t brightness_ = (uint8_t)(brightness*250.0);
    set_power(brightness_);
}

}
}