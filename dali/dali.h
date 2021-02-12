#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace dali {

#define LOG_I2C_DEVICE(this) ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);

#define DALI_RX_READ(X)  (X->digital_read() == HIGH ? 1 : 0)
#define DALI_TX_LOW(X)   X->digital_write(LOW)
#define DALI_TX_HIGH(X)  X->digital_write(HIGH)

// Query commands
#define READ_MEMORY_LOCATION 197 // CMD:257

#define DALI_RECV_RES_OK  0
#define DALI_RECV_RES_IDLE  1
#define DALI_RECV_RES_TOO_LONG  2

/** The DALIComponent is the base of ESPHome's i2c communication.
 *
 */
class DALIDevice;
class DALIComponent : public Component {
public:
  static DALIComponent *singleton;
	static DALIComponent *getInstance() { return singleton; }
 public:
  DALIComponent();
  void register_device(DALIDevice *device);
  DALIDevice *get_device(uint8_t num) { return num<mDevices.size() ? mDevices[num] : nullptr; }
  void set_tx_pin(GPIOPin *tx_pin) { mTxPin = tx_pin; }
  void set_rx_pin(GPIOPin *rx_pin) { mRxPin = rx_pin; }
  void set_frequency(uint32_t frequency);
  void set_scan(bool scan) { scan_ = scan; }
  /// Setup the i2c. bus
  void setup() override;
  void dump_config() override;
  /// Set a very high setup priority to make sure it's loaded before all other hardware.
  float get_setup_priority() const override;
private:
  void send_start_bit();
  void send_one_bit();
  void send_zero_bit();
  void send_idle_bit();
private:
  uint32_t recv_deltatime(uint32_t start, uint32_t end) const;
  bool sample_rx_line() const { return DALI_RX_READ(mRxPin); }
public:
  void send_forward(uint8_t a, uint8_t d);
  uint8_t recv_backward(uint8 *data);
  void dali_pause(uint8_t numte);
protected:
  GPIOPin *mTxPin{nullptr};
  GPIOPin *mRxPin{nullptr};
  uint32_t frequency_;
  uint32_t delay_;
  bool scan_ = false;
  friend class DALIDevice;
  std::vector<DALIDevice *> mDevices;
};

class DALIDevice {
public:
  DALIDevice() = default;
  DALIDevice(DALIComponent *parent, uint8_t address): mAddress(address), mParent(parent) { }

  /// Manually set the i2c address of this device.
  void set_dali_address(uint8_t address);
  /// Manually set the parent i2c bus for this device.
  void set_dali_parent(DALIComponent *parent);
public:
  void send_forward(uint8_t a, uint8_t d) { mParent->send_forward(a, d); }
  uint8_t recv_backward(uint8 *data) { return mParent->recv_backward(data); }
  void dali_pause(uint8_t numte) { mParent->dali_pause(numte); }
public:
  virtual void set_power(uint8_t power);
protected:
  uint8_t mAddress;
  DALIComponent *mParent{nullptr};
  uint8_t mPower = 192;
};

}  // namespace i2c
}  // namespace esphome
