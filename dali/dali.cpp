#include "dali.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/application.h"

#include <user_interface.h>
#include <osapi.h>

void ets_delay_us(uint32_t us);

namespace esphome {
namespace dali {

static const char *TAG = "dali";

DALIComponent * DALIComponent::singleton = nullptr;

DALIComponent::DALIComponent() {
  DALIComponent::singleton = this;
}

void DALIComponent::register_device(DALIDevice *device) {
  mDevices.push_back(device);
}

void DALIComponent::setup() {
  mTxPin->setup();
  mRxPin->setup();
}

void DALIComponent::set_frequency(uint32_t frequency) {
  frequency_ = frequency; 
  delay_ = (int)(1.0f/(float)frequency_*1000000.0f);
}

void DALIComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "DALI Bus:");
  ESP_LOGCONFIG(TAG, "  TX Pin: GPIO%u mode %s", mTxPin->get_pin(), mTxPin->get_pin_mode_name());
  ESP_LOGCONFIG(TAG, "  RX Pin: GPIO%u mode %s", mRxPin->get_pin(), mRxPin->get_pin_mode_name());
  ESP_LOGCONFIG(TAG, "  Frequency: %u Hz", this->frequency_);
  if (this->scan_) {
    ESP_LOGI(TAG, "Scanning DALI bus for active devices...");
    uint8_t found = 0;
    for (uint8_t address = 1; address < 120; address++) {
      delay(1);
    }
    if (found == 0) {
      ESP_LOGI(TAG, "Found no dali devices!");
    }
  }
}
float DALIComponent::get_setup_priority() const { return setup_priority::BUS; }

void DALIComponent::send_start_bit() {
  DALI_TX_HIGH(mTxPin);
  os_delay_us(delay_);
  DALI_TX_LOW(mTxPin);
  os_delay_us(delay_);
}

void DALIComponent::send_one_bit() {
  DALI_TX_HIGH(mTxPin);
  os_delay_us(delay_);
  DALI_TX_LOW(mTxPin);
  os_delay_us(delay_);
}

void DALIComponent::send_zero_bit() {
  DALI_TX_LOW(mTxPin);
  os_delay_us(delay_);
  DALI_TX_HIGH(mTxPin);
  os_delay_us(delay_);
}

void DALIComponent::send_idle_bit() {
  DALI_TX_LOW(mTxPin);
  os_delay_us(delay_);
  DALI_TX_LOW(mTxPin);
  os_delay_us(delay_);
}

uint32_t DALIComponent::recv_deltatime(uint32_t start, uint32_t end) const {
    uint32_t delta;
    if(end > start) delta = end - start;
    else delta = 0xFFFFFFFF - (start - end) + 1;
    return delta;
}

void DALIComponent::send_forward(uint8_t a, uint8_t d) {
  uint8_t i = 0;
  send_start_bit();
  for(i=0;i<8;i++) {
      if((a&0x80) > 0) send_one_bit(); else send_zero_bit();
      a <<= 1;
  }
  for(i=0;i<8;i++) {
      if((d&0x80) > 0) send_one_bit(); else send_zero_bit();
      d <<= 1;
  }
  send_idle_bit();
  send_idle_bit();
}

#define BITS_SIZE 64

uint8_t DALIComponent::recv_backward(uint8 *data) {
    uint8_t result = DALI_RECV_RES_OK;
    bool waitstart = true;

    uint32 currtime;
    uint32 frametime = 0;
    uint32 starttime = system_get_time();

    int8_t curr_bit_index = -1;
    uint8_t lasttiming;
    uint8_t timings[BITS_SIZE];
    uint8_t manchester[BITS_SIZE];
    uint8_t curr_manchester_index = 0;

    uint8_t i;
    for(i=0; i<BITS_SIZE;i++) timings[i] = 0;

    while(true) {
        currtime = system_get_time();

        // Global packet time-out 22Te + 22Te
        if(recv_deltatime(starttime, currtime) > (9170*2)) {
            result = DALI_RECV_RES_IDLE;
            break;
        }

        uint8_t bit = sample_rx_line();

        if(waitstart) {
            if(bit) {
                // Detected start transtion start frame time
                waitstart = false;
                lasttiming = bit;
                curr_bit_index++;
                timings[curr_bit_index]++;
                frametime = system_get_time();
            }
        } else {
            if(bit != lasttiming) {
                curr_bit_index++;
                if(curr_bit_index >= BITS_SIZE) {
                    curr_bit_index--;
                    result = DALI_RECV_RES_TOO_LONG;
                    break;
                }
                lasttiming = bit;
            }

            timings[curr_bit_index]++;
            // Check if but is too long (transmission terminated)
            if(timings[curr_bit_index]>0xF0) {
                // This timing is will not deocded
                curr_bit_index--;
                break;
            }
        }

        os_delay_us(10);
    }
    // curr_bit_index is size of array
    curr_bit_index++;

    uint32_t totalduration = recv_deltatime(starttime, currtime);
    uint32_t frameduration = recv_deltatime(frametime, currtime);
    //ESP_LOGI(TAG, "recv_backwoard result %d size %d total time %d frame time %d", result, curr_bit_index, totalduration, frameduration);

    if(curr_bit_index>0) {
        uint8_t readed = 0;
        uint8_t bit = 0;

        // Find the manchester encoding
        for(i=0;i<curr_bit_index;i++) {
            manchester[curr_manchester_index++] = bit;
            if(timings[i]>0x34) manchester[curr_manchester_index++] = bit;
            bit = bit==0 ? 1 : 0;
        }
        // Id odd we have to add last
        if(curr_manchester_index % 2 == 1) manchester[curr_manchester_index++] = 1;

        bit = 0x80;
        for(i=0;i<curr_manchester_index;i+=2) {
            if(i==0) {
                if(manchester[i]!=0 && manchester[i+1]!=1) {
                    ESP_LOGD(TAG, "Missing start byte");
                }
            } else {
                //if(manchester[i]==1 && manchester[i+1]==0) { }
                if(manchester[i]==0 && manchester[i+1]==1) readed |= bit;
                //if((manchester[i]==0 && manchester[i+1]==0) || (manchester[i]==1 && manchester[i+1]==1))
                    //DEBUG_MSG("Invalid manchester encoding at %d (%d,%d)\n", i, manchester[i], manchester[i+1]);
                bit >>= 1;
            }
        }

        //ESP_LOGD(TAG, "dali_recv_backwoard readed %d", readed);
        data[0] = readed;
    }

    return result;
}

void DALIComponent::dali_pause(uint8_t numte) {
    for(int i=0; i<numte; i++) {
        os_delay_us(416);
        App.feed_wdt();
    }
}


void DALIDevice::set_dali_address(uint8_t address) { mAddress = address; }
void DALIDevice::set_dali_parent(DALIComponent *parent) { mParent = parent; }

void DALIDevice::set_power(uint8_t power) {
  mPower = power;
  send_forward(0xFE, mPower);
}


}  // namespace i2c
}  // namespace esphome
