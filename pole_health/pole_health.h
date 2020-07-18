#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace polehealth {

#define AVERAGE_FILTER_HISTORY 10

class PoleHealthSensor : public Component, public sensor::Sensor {
  enum AccellerometerDirection {DirAccelX=0, DirAccelY, DirAccelZ};
public:
  void set_accel_x(Sensor *sensor) { mAccelX = sensor; }
  void set_accel_y(Sensor *sensor) { mAccelY = sensor; }
  void set_accel_z(Sensor *sensor) { mAccelZ = sensor; }
  void set_roation_sensor(Sensor *sensor) { mInclinometer = sensor; }

  void setup() override;
  virtual void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
protected:
  void process_(float value, AccellerometerDirection dir);
private:
  void searchForImpacts();
  unsigned long elapsedMillis(unsigned long t2, unsigned long t1) const { return t2 >= t1 ? t2 - t1 : (~(t1 - t2)) + 1; }
private:
  sensor::Sensor *mAccelX;
  sensor::Sensor *mAccelY;
  sensor::Sensor *mAccelZ;
private:
  sensor::Sensor *mInclinometer;
private:
  float mStates[3];
  float mLowPassStates[3];
  uint32_t mSensorState = 0;
  uint32_t mLoopTime = 0;
  int mSensorsIterations = 0;
  float mSensorThreshold = 0.5;
  int mOverThresholdMax = 3;
  int mOverThresholdCurr = 0;
  int mOverThresholdLast = 0;
  bool mOverThresholdIsNew = true;
};

}  // namespace polehealth
}  // namespace esphome
