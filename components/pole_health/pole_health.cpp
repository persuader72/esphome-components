#include "pole_health.h"
#include "esphome/core/log.h"

namespace esphome {
namespace polehealth {

static const char *TAG = "polehealth";

void PoleHealthSensor::setup() {
  for(int i=0; i<3; i++) mStates[i] = NAN;
  this->mAccelX->add_on_state_callback([this](float value) { this->process_(value, DirAccelX); });
  if (this->mAccelX->has_state()) this->process_(this->mAccelX->state, DirAccelX);
  this->mAccelY->add_on_state_callback([this](float value) { this->process_(value, DirAccelY); });
  if (this->mAccelY->has_state()) this->process_(this->mAccelZ->state, DirAccelY);
  this->mAccelZ->add_on_state_callback([this](float value) { this->process_(value, DirAccelZ); });
  if (this->mAccelZ->has_state()) this->process_(this->mAccelZ->state, DirAccelZ);

  set_internal(true);
  if(mInclinometer != nullptr) mInclinometer->set_internal(true);
  mLoopTime = millis();
}

void PoleHealthSensor::loop() {
  uint32_t now = millis();
  if(elapsedMillis(now, mLoopTime) > 10000) {
    ESP_LOGD(TAG, "PoleHealthSensor::loop val:%d times:%d isnew %d", mSensorState, mOverThresholdLast, mOverThresholdIsNew);
    mLoopTime = now;
  }
}

void PoleHealthSensor::dump_config() {
  LOG_SENSOR(TAG, "PoleHealth Sensor", this);
}

void PoleHealthSensor::process_(float value, AccellerometerDirection dir) {
  if (isnan(value)) {
    this->publish_state(mSensorState);
    return;
  }

  mStates[(int)dir] = value;

  if(!isnan(mStates[DirAccelX]) && !isnan(mStates[DirAccelY]) && !isnan(mStates[DirAccelZ])) {
    mLowPassStates[DirAccelX] += (mStates[DirAccelX]-mLowPassStates[DirAccelX])/10;
    mLowPassStates[DirAccelY] += (mStates[DirAccelY]-mLowPassStates[DirAccelY])/10;
    mLowPassStates[DirAccelZ] += (mStates[DirAccelZ]-mLowPassStates[DirAccelZ])/10;

    float x_square = mStates[DirAccelX] * mStates[DirAccelX];
    float y_square = mStates[DirAccelY] * mStates[DirAccelY];
    float z_square = mStates[DirAccelZ] * mStates[DirAccelZ];
      

    float pitch = atan(-mStates[DirAccelX] / sqrt(y_square+z_square)) * 180.0 / PI;
    float roll = atan(mStates[DirAccelY] / (mStates[DirAccelZ]>=0 ? 1.0 : -1.0) / sqrt(z_square + x_square * 0.01)) * 180.0 / PI;

    if (this->mInclinometer != nullptr)
    this->mInclinometer->publish_state(pitch >= roll ? pitch : roll);
    
    // Diamo temp al filtro passa basso di stabilizarsi
    if(mSensorsIterations < 100) mSensorsIterations++; else searchForImpacts();
    for(int i=0; i<3; i++) mStates[i] = NAN;
  }

  float res = 0;
  this->publish_state(mSensorState);
}

void PoleHealthSensor::searchForImpacts() {
  float dx = mStates[DirAccelX] - mLowPassStates[DirAccelX];
  float dy = mStates[DirAccelY] - mLowPassStates[DirAccelY];
  float dz = mStates[DirAccelZ] - mLowPassStates[DirAccelZ];
  float deltamod = sqrt(dx*dx+dy*dy+dz*dz);

  if(deltamod >= mSensorThreshold) {
    // Se non ho superato la solgia incremnto il contatore
    if(mOverThresholdCurr <= mOverThresholdMax) mOverThresholdCurr++;
    // Se ho superato la soglia
    if(mOverThresholdCurr > mOverThresholdMax && mOverThresholdIsNew) {
      // Segno il valore per uso statistico
      mOverThresholdLast = mOverThresholdCurr;
      // Incremento il contatore degli stati
      mSensorState++;
      // Mi segno che ho gia contato l'evento per non contarlo più volte.
      mOverThresholdIsNew = false;
    }
  } else if(mOverThresholdCurr > 0) {
    // Non ho suerato la soglia ma in precedenza si
    if(--mOverThresholdCurr == 0) {
      // E passato un temp sufficiente esono in quiete posso cercae un nuovo evento
      mOverThresholdIsNew = true;
    }
  }
}

}  // namespace resistance
}  // namespace esphome
