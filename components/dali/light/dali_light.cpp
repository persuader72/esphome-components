#include "dali_light.h"
#include "esphome/core/log.h"
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"

namespace esphome {
namespace dali {

static const char *TAG_LIGHT = "dali::light";

void DALILight::setup() {
  mLastPowerRefresh = millis();
}

void DALILight::loop() {
  uint32_t now = millis();
  if(elapsedMillis(now, mLastPowerRefresh) > 2000) {
    mLastPowerRefresh = now;
    set_power(mPower);
  }
}

light::LightTraits DALILight::get_traits() {
  auto traits = light::LightTraits();
  traits.set_supported_color_modes({light::ColorMode::BRIGHTNESS});
  return traits;
}

void DALILight::setup_state(light::LightState *state) {
  state->make_call().set_brightness(0.8).perform();
}

void DALILight::write_state(light::LightState *state) {
    bool binary;
    float brightness;

    state->current_values_as_binary(&binary);
    state->current_values_as_brightness(&brightness);

    //ESP_LOGI(TAG_LIGHT, "write_state binary %d brightness %f", binary, brightness);

    if(!binary || brightness==0) {
      set_power(0);
      return;
    }

    uint8_t brightness_ = (uint8_t)(brightness*250.0);
    set_power(brightness_);
}

}
}
