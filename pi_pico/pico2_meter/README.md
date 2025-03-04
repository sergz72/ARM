# Raspberry PI pico2 universal measuring tool

**Features:**
1. Supports 3 10pin modules and 2 14pin modules.
2. External I2C device support:
  - SI5351 DDS generator, Frequency range: 7813Hz...160MHz
  - INA226 current shunt and power monitor. Voltage range is 0..36v, resolution is 1mV, Current range with 0.1Ohm shunt resistor is -0.8..0.8A, resolution: 0.1mA
  - MCP3421 ADC with voltage divider 1:11 (1MOhm resistor from input to ADC, 100kOhm resistor+0.1uf capacitor from ADC to GND). Range is -4..+33v. Resolution is 0.01mV. Calibration is supported.
  - MCP9600/9601 Thermocouple EMF to Temperature Converter.
  - ADS1115 ADC.
  - INA3221  Triple-Channel, High-Side Measurement, Shunt and Bus Voltage Monitor.
  - 2-channel 1Hz-37MHz frequency counter.
  - 2-channel 1Hz-25MHz PWM generator.
3. External SPI modules support:
  - [AD9833 DDS](../../stm32g0/stm32g0_ad9833)
  - [AD9850 DDS](../../stm32g0/stm32g0_ad9833)
  - [4 channel DDS with 297 MHz clock, 4 channel PWM with 74.25 MHz clock, 2 channel frequency counter using Tang Primer 20k FPGA](https://github.com/sergz72/FPGA/tree/main/multidevice)

I2C/SPI interface selection: when PIN_SDA has logic level 1 during application boot - that means I2C interface, logic 0 - SPI

**Pins:**

//module 1 (14 pin)
PIN_SDA1/SPI_MOSI 2
PIN_SCL1/SPI_SCK  3
PIN_1_0/SPI_MISO  4
PIN_1_1/SPI_NCS   5
PIN_1_2/INTERRUPT 0
PIN_1_3  1

//module 2  (10 pin)
PIN_SDA2/SPI_MOSI 6
PIN_SCL2/SPI_SCK  7
PIN_2_0/SPI_MISO  8
PIN_2_1/SPI_NCS   9

//module 3  (10 pin)
PIN_SDA3/SPI_MOSI 12
PIN_SCL3/SPI_SCK  13
PIN_3_0/SPI_MISO  14
PIN_3_1/SPI_NCS   15

// module 4  (10 pin)
PIN_SDA4/SPI_MOSI 17
PIN_SCL4/SPI_SCK  16
PIN_4_0/SPI_MISO  19
PIN_4_1/SPI_NCS   18

// module 5  (14 pin)
PIN_SDA5/SPI_MOSI 26
PIN_SCL5/SPI_SCK  22
PIN_5_0/SPI_MISO  28
PIN_5_1/SPI_NCS   27
PIN_5_2/INTERRUPT 21
PIN_5_3  20
