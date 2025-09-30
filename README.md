# persuader72's esphome custom components

This repository contains a collection of my custom components.

## 1. Installation

Check [this](https://esphome.io/components/external_components/) page to find out how to use external components

```yaml
external_components:
  - source: github://persuader72/esphome-components@master
```

## 2. Components

### shelly.one_wire

External component for to support [Temperature Sensor AddOn for Shelly 1/1PM](https://www.shelly.si/en/accessories-for-shelly-products/67-shelly-plus-addon-3800235266427.html), derived from the original [one_wire](https://github.com/esphome/esphome/tree/dev/esphome/components/gpio/one_wire) component from [esphome](https://github.com/esphome/). 

```yaml
external_components:
  - source: github://persuader72/esphome-components@master
    components: [ shelly ]

logger:
  level: INFO
  baud_rate: 0

one_wire:
  - platform: shelly
    input: GPIO1
    output: GPIO0

sensor:
  - platform: dallas_temp
    name: temperature
    update_interval: 120s

```

### shelly_dallas component

A component to support [Temperature Sensor AddOn for Shelly 1/1PM](https://shop.shelly.cloud/temperature-sensor-addon-for-shelly-1-1pm-wifi-smart-home-automation#312), derived from the original [dallas](https://github.com/esphome/esphome/tree/dev/esphome/components/dallas) component from [esphome](https://github.com/esphome/). 

To configure it is possible to follow the same schema found dallas [component documentation](https://esphome.io/components/sensor/dallas.html) with the following differences:

* shelly_dallas must be used as platform
* the pin configuration must be configured as in the following example

To find the sensor ID follow the same procedure in found in dallas component: https://esphome.io/components/sensor/dallas.html

```yaml
shelly_dallas:
  pin_a: GPIO3
  #pin_a: GPIO1 #For Shelly Plus 2PM
  pin_b: GPIO0

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
