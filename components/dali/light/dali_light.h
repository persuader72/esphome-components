#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/components/dali/dali.h"
#include "esphome/components/light/light_output.h"

namespace esphome {
namespace dali {


class DALILight : public Component, public DALIDevice, public light::LightOutput {
public:
    void setup() override;
    void loop() override;
public:
    /// Return the LightTraits of this LightOutput.
    virtual light::LightTraits get_traits();
    virtual void setup_state(light::LightState *state) override;
    virtual void write_state(light::LightState *state);
private:
    unsigned long elapsedMillis(unsigned long t2, unsigned long t1) { return t2 >= t1 ? t2 - t1 : (~(t1 - t2)) + 1; }
private:
    uint32_t mLastPowerRefresh = 0;
};

}  // namespace i2c
}  // namespace esphome
