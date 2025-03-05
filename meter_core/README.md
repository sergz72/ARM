# Core formware for [Universal measuring tool](../meter_ui)

## Features

1. Supports 10/14 pin modules
2. External I2C device support:
  - SI5351 DDS generator, Frequency range: 7813Hz...160MHz
  - INA226 current shunt and power monitor. Voltage range is 0..36v, resolution is 1mV, Current range with 0.1Ohm shunt resistor is -0.8..0.8A, resolution: 0.1mA
  - MCP3421 ADC with voltage divider 1:11 (1MOhm resistor from input to ADC, 100kOhm resistor+0.1uf capacitor from ADC to GND). Range is -4..+33v. Resolution is 0.01mV.
  - MCP9600/9601 Thermocouple EMF to Temperature Converter.
  - ADS1115 ADC.
  - INA3221  Triple-Channel, High-Side Measurement, Shunt and Bus Voltage Monitor.
3. External SPI modules support:
  - [AD9833 DDS](../../stm32g0/stm32g0_ad9833)
  - [AD9850 DDS](../../stm32g0/stm32g0_ad9833)
  - Signal level meters like AD8307, AD8318, etc (together with a DDS module).
  - [4 channel DDS with 297 MHz clock, 4 channel PWM with 74.25 MHz clock, 2 channel frequency counter using Tang Primer 20k FPGA](https://github.com/sergz72/FPGA/tree/main/multidevice)

## 10 pin module pinout

| Pin(s) | Signal name        |
| :---:  | :---:              |
| 1,2    | 5v                 |
| 3,4    | 3.3v               |
| 5      | SDA/MOSI           |
| 6      | SCL/SCK            |
| 7      | IO0/INTERRUPT/MISO |
| 8      | IO1/NCS            |
| 9,10   | GND                |

## 14 pin module pinout

| Pin(s)  | Signal name        |
| :---:   | :---:              |
| 1       |                    |
| 2       | RESET (active low) |
| 3       | IO2/INTERRUPT      |
| 4       | IO3                |
| 5,6     | 5v                 |
| 7,8     | 3.3v               |
| 9       | SDA/MOSI           |
| 10      | SCL/SCK            |
| 11      | IO0/INTERRUPT/MISO |
| 12      | IO1/NCS            |
| 13,14   | GND                |

## I2C/SPI interface selection: when SDA pin has logic level 1 during application boot - that means I2C interface, logic 0 - SPI
## Interrupt pin active level is high. For I2C modules IO0 is an interrupt pin. For SPI modules - IO2 (if present).
