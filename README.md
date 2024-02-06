# Bastlířská KPZ – M5KPZ

M5KPZ is a suite of applications for the m5stick-c-plus utilizing M5 unity and sensors. It operates as a single
application within which you can easily switch between different "modules/applications".

![](./docs/kpz.jpeg)

## Compatibility

M5KPZ can be run on multiple platforms. You can easily switch between platforms using platformio environments.
Currently, the following devices are supported:

- **m5stick-c-plus**: Build for M5StickC-Plus
- **picopad**: Build for Picopad
- **emulator**: Desktop SDL2 emulator

## Controls

### Button A

- Open the displayed module of the application
- Actions within individual applications

### Button B

- Scroll through applications/modules
- Change screen brightness within an application/module

### PWR Button

- Exit the application or module and return to the list of applications

### Button Mapping for Picopad

- Button A = Button A
- Button B = Button B
- PWR Button = Button X
- Button Y – Reset to bootloader

### Button Mapping for Emulator

- Button A = Left Arrow
- Button B = Down Arrow
- PWR Button = Up Arrow

## Module List

The graphical interface for individual applications is adapted and inspired by the amazing Volos project ESP32 Li-ion
Battery Charger: [Volos Project - ChargerM5StickC](https://github.com/VolosR/chargerM5StickC).

## Sensors
- **ENV HAT**: [ENV HAT Documentation](https://docs.m5stack.com/en/hat/hat-env)
- **ENV III Unit**: [ENV III Documentation](https://docs.m5stack.com/en/unit/envIII)
- **NCIR2 Unit**: [NCIR2 Documentation](https://docs.m5stack.com/en/unit/NCIR2)
- **TVOC/eCO2 Unit**: [TVOC/eCO2 Documentation](https://docs.m5stack.com/en/unit/tvoc)
- **ToF HAT**: [ToF HAT Documentation](https://docs.m5stack.com/en/hat/hat-tof)
- **GPS Unit**: [GPS Documentation](https://docs.m5stack.com/en/unit/gps)
- **PIR Unit**: [PIR Documentation](https://docs.m5stack.com/en/unit/PIR)
- **Voltmeter Unit**: [Voltmeter Documentation](https://docs.m5stack.com/en/unit/vmeter)
- **IR Decoder**: Uses IR unit [IR Unit Documentation](https://docs.m5stack.com/en/unit/ir)
- **INA219 DC Current Sensor**: 26V ±3.2A [Adafruit Documentation](https://learn.adafruit.com/adafruit-ina219-current-sensor-breakout)

## Tools

- **Torch**: Uses FlashLight Unit [FlashLight Unit Documentation](https://docs.m5stack.com/en/unit/FlashLight)
- **Flash Light**: Uses FlashLight Unit [FlashLight Unit Documentation](https://docs.m5stack.com/en/unit/FlashLight)
- **Camera Shutter**: A mobile camera shutter controlled via BLE, simulating volume down button through BLE Keyboard.
  Tested only with iOS. (Not part of the build for Picopad)
- **Stopwatch**: Adapted from Volos Projects [M5StickStopwatch](https://github.com/VolosR/M5StickStopwatch). Uses
  Dual-BUTTON Unit [Dual-BUTTON Documentation](https://docs.m5stack.com/en/unit/dual_button)
- **Resistor CALC**: Adapted from IcingTomato M5Stick C Plus colour-ring resistor
  calculator - [Resistor Calculator](https://github.com/IcingTomato/M5StickC-Resistor-Calculator).
- **TV Remote**: A simple IR remote for Samsung TVs. Uses IR
  unit [IR Unit Documentation](https://docs.m5stack.com/en/unit/ir).

## Games

- **Dices**: Adapted from sample examples
  at [M5StickC-Plus Dices](https://github.com/m5stack/M5StickC-Plus/tree/master/examples/Games/Dices)
- **Brake Out Game**: Adapted from Volos
  Projects [M5StickCPlusBrakeout](https://github.com/VolosR/M5StickCPlusBrakeout/tree/main)
- **Tetris**: Adapted from Volos Projects [M5ScickPlusTetris](https://github.com/VolosR/M5ScickPlusTetris/blob/main/)

## Screens

### Sensors

![](./docs/01-evniii-1.png)
![](./docs/01-evniii-2.png)
![](./docs/02-ncir-1.png)
![](./docs/02-ncir-2.png)
![](./docs/03-tvoc-1.png)
![](./docs/03-tvoc-2.png)
![](./docs/04-tof-1.png)
![](./docs/04-tof-2.png)
![](./docs/05-gps-1.png)
![](./docs/05-gps-2.png)
![](./docs/06-pir-1.png)
![](./docs/06-pir-2.png)
![](./docs/07-voltmeter-1.png)
![](./docs/07-voltmeter-2.png)
![](./docs/08-ir-decoder-1.png)
![](./docs/08-ir-decoder-2.png)

### Tools

![](./docs/09-torch-1.png)
![](./docs/09-torch-2.png)
![](./docs/10-flash-light-1.png)
![](./docs/10-flash-light-2.png)
![](./docs/11-shutter-1.png)
![](./docs/11-shutter-2.png)
![](./docs/12-stopwatch-1.png)
![](./docs/12-stopwatch-2.png)
![](./docs/13-resistor-calc-1.png)
![](./docs/13-resistor-calc-2.png)
![](./docs/14-tv-remote-1.png)
![](./docs/14-tv-remote-2.png)

### Games

![](./docs/15-dices-1.png)
![](./docs/15-dices-2.png)
![](./docs/15-dices-3.png)
![](./docs/16-brakeout-1.png)
![](./docs/16-brakeout-2.png)
![](./docs/16-brakeout-3.png)
![](./docs/17-tetris-1.png)
![](./docs/17-tetris-2.png)
![](./docs/17-tetris-3.png)
