#include "lp5024.h"
#include "esphome/core/log.h"

namespace esphome {
namespace lp5024 {

static const char *TAG = "lp5024";

#define LP5024_ERROR_CHECK(func) \
  if (!func) { \
    this->mark_failed(); \
    return; \
  }
#define LP5024_WRITE_BYTE(reg, value) LP5024_ERROR_CHECK(this->write_byte(reg, value));

void LP5024LightOutputBase::setup() {
  ESP_LOGCONFIG(TAG, "Setting up LP5024...");
  uint8_t id;
  if (!this->read_byte(0x01, &id)) {  // ID register
    this->mErrorCode = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  this->write_byte(0x00, 0x40);
  this->write_byte(0x02, 0x00);
  this->write_byte(0x07, 0x80);

  for(int i=0; i<8; i++) {
    mLeds[i].r = 0x20;
    mLeds[i].g = 0x05;
    mLeds[i].b = 0x05;
  }

  ESP_LOGCONFIG(TAG, "Setting up LP5024... %d", id);
}

void LP5024LightOutputBase::dump_config() {
  ESP_LOGCONFIG(TAG, "LP5024:");
  LOG_I2C_DEVICE(this);

  if (this->is_failed()) {
    switch (this->mErrorCode) {
      case COMMUNICATION_FAILED:
        ESP_LOGE(TAG, "Communication with LP5024 failed!");
        break;
      case WRONG_ID:
        ESP_LOGE(TAG, "LP5024 has invalid id!");
        break;
      default:
        ESP_LOGE(TAG, "Setting up LP5024 registers failed!");
        break;
    }
  }
}

#define APDS9960_WARNING_CHECK(func, warning) \
  if (!(func)) { \
    ESP_LOGW(TAG, warning); \
    this->status_set_warning(); \
    return; \
  }

void LP5024LightOutputBase::loop() { 
  //static uint8_t reg = 0;
  //APDS9960_WARNING_CHECK(this->read_byte(0x93, &status), "Reading status bit failed.");
  //this->status_clear_warning();
  /*uint32 now = millis();
  if(LP5024LightOutputBase::elapsedMillis(now, mLastTime) > 250)  {
    uint8_t regval;
    this->read_byte(reg, &regval);
    ESP_LOGD(TAG, "%d %d", reg, regval);
    if(reg>=0x0F && reg<=0x26) {
      regval = regval==0 ? 0x80 : 0x00;
      this->write_byte(reg, regval);
    }
    mLastTime = now;
    reg++;
    if(reg > 0x26) reg=0;
  }*/

  if (this->should_show_()) {
    int j=0xF;
    for(int i=0;i<8;i++) {
      this->write_byte(j++, mLeds[i].r);
      this->write_byte(j++, mLeds[i].g);
      this->write_byte(j++, mLeds[i].b);
    }
    this->mark_shown_();
  }
}


int32_t LP5024LightOutputBase::size() const {
  return 8;
}

void LP5024LightOutputBase::clear_effect_data() {
}

light::LightTraits LP5024LightOutputBase::get_traits() { 
    auto traits = light::LightTraits();
    traits.set_supports_brightness(true);
    traits.set_supports_rgb(true);
    return traits;
}

}  // namespace apds9960
}  // namespace esphome
