#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace mitsubishi_general {

class MitsubishiGeneralClimate : public climate_ir::ClimateIR {
public:
  MitsubishiGeneralClimate();
public:
  bool on_receive(remote_base::RemoteReceiveData data) override;
protected:
  /// Override control to change settings of the climate device.
  void control(const climate::ClimateCall &call) override;
  /// Return the traits of this controller.
  climate::ClimateTraits traits() override;
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;

  bool power_{false};
  uint8_t mFanSpeed = 0;
  uint8_t mSwingMode = 0;
};

}  // namespace mitsubishi_general
}  // namespace esphome
