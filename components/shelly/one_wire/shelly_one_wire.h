#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/one_wire/one_wire.h"

namespace esphome {
namespace shelly {

class ShellyOneWireBus : public one_wire::OneWireBus, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::BUS; }

  void set_pins(InternalGPIOPin *pin_in, InternalGPIOPin *pin_out) {
    this->t_pin_in = pin_in;
    this->pin_in = pin_in->to_isr();
    this->t_pin_out = pin_out;
  }

  void write8(uint8_t val) override;
  void write64(uint64_t val) override;
  uint8_t read8() override;
  uint64_t read64() override;

 protected:
  InternalGPIOPin *t_pin_in;
  ISRInternalGPIOPin pin_in;
  InternalGPIOPin *t_pin_out;
  uint8_t last_discrepancy_{0};
  bool last_device_flag_{false};
  uint64_t address_;

  int reset_int() override;
  void reset_search() override;
  uint64_t search_int() override;
  void write_bit_(bool bit);
  bool read_bit_();
  bool read_bit_(uint32_t *t);
};

}  // namespace shelly
}  // namespace esphome
