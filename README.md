## shelly_dallas component

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

To find the sesnor ID follow the same procedure in https://esphome.io/components/sensor/dallas.html


