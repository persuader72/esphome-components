# persuader72's esphome custom components

This repository contains a collection of my custom components.

## 1. Installation

Clone this repository into `custom_components` in a folder where the your config `yaml` is stored. 

```bash
git clone https://github.com/persuader72/esphome-components custom_components
```

Or copy a single selected component folder in your already exiting `custom_components` folder.

## 2. Components

### shelly_dallas component

A component to support [Temperature Sensor AddOn for Shelly 1/1PM](https://shop.shelly.cloud/temperature-sensor-addon-for-shelly-1-1pm-wifi-smart-home-automation#312), derived from the original [dallas](https://github.com/esphome/esphome/tree/dev/esphome/components/dallas) component from [esphome](https://github.com/esphome/). 

To configure it is possible to follow the same schema found dallas [component documentation](https://esphome.io/components/sensor/dallas.html) with the following differences:

* shelly_dallas must be used as platform
* the pin configuration must be configured as in the following example

To find the sensor ID follow the same procedure in found in dallas component: https://esphome.io/components/sensor/dallas.html

```yaml
shelly_dallas:
  pin: GPIO3
  pin_a: GPIO0

sensor:
  - platform: shelly_dallas
    address: 0xCA011938282E5128
    name: "Temperature #1"
  - platform: shelly_dallas
    address: 0x0B01193807DD3128
    name: "Temperature #2"
  - platform: shelly_dallas
    address: 0x6E0119384DC70728
    name: "Temperature #3"
```

### shelly_dht component

A patch to the [dht](https://github.com/esphome/esphome/tree/dev/esphome/components/dht) component from [esphome](https://github.com/esphome/) to make it compatible with the  [Temperature Sensor AddOn for Shelly 1/1PM](https://shop.shelly.cloud/temperature-sensor-addon-for-shelly-1-1pm-wifi-smart-home-automation#312).

```yaml
sensor:
  - platform: shelly_dht
    pin: GPIO3
    pin_a: GPIO0
    temperature:
      name: "Living Room Temperature"
    humidity:
      name: "Living Room Humidity"
    update_interval: 60s
```
