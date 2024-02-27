#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace hisense_ac {

class HisenseAcClimate : public climate_ir::ClimateIR {
public:
  HisenseAcClimate();
public:
  bool on_receive(remote_base::RemoteReceiveData data) override;
protected:
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;

  bool power_{false};
};

}  // namespace hisense_ac
}  // namespace esphome
