# STM32401 Blue pill universal measuring tool

![](stm32f401_meter.jpg)

**Supports:**
1. Up to 4 external I2C devices:
  - SI5351 DDS generator. Frequency range: 7813Hz...112.5MHz
  - INA226 current shunt and power monitor. Voltage range is 0..36v, resolution is 1mV, Current range with 0.1Ohm shunt resistor is -0.8..0.8A, resolution: 0.1mA
  - INA3221 three-channel, high-side current and bus voltage monitor. Voltage range is 0..26v, resolution is 1mV, Current range with 0.1Ohm shunt resistor is -1.6..1.6A, resolution: 0.1mA
  - ADS1115 4-channel, delta-sigma ADC with with voltage divider 1:10. Resolution is 1mV
  - MCP3421 ADC with voltage divider 1:11 (1MOhm resistor from input to ADC, 100kOhm resistor+0.1uf capacitor from ADC to GND). Range is -4..+33v. Resolution is 0.01mV
  - Generic DDS I2C device (example device: https://github.com/sergz72/MSP430/blob/main/generator_ad9833)
2. Optionally [TM1638 LED/keyboard](https://www.amazon.de/gp/product/B07Y9QB74M)

**Uses:**
- [STM32F401 Development Board](https://www.amazon.de/gp/product/B0B5DVM7KH)
- 128x64 lcd module with ks0108 controller
- Optionally [TM1638 LED/keyboard](https://www.amazon.de/gp/product/B07Y9QB74M)

**Pins:**

**128x64 LCD:**
- VDD: 5v
- DI: PC14
- RW: GND
- E: PC15
- D0: PA0
- D1: PA1
- D2: PA2
- D3: PA3
- D4: PA4
- D5: PA5
- D6: PA6
- D7: PA7
- CS1: PB0
- CS2: PB1
- RES: PB2

**I2C:**

Channel1:
- SDA: PB10
- SCL: PB3

Channel2:
- SDA: PA8
- SCL: PB4

Channel3:
- SDA: PB6
- SCL: PB7

Channel4:
- SDA: PB8
- SCL: PB9
- GPIO: PA15, PB5

**Optional TM1638 module connection(Uses I2C channel 4 pins):**
- PB9: GND
- PB8: DIO
- PA15: CLK
- PB5: CS
