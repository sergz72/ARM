# Raspberry PI pico2 board for [Universal measurements tool](../../meter_ui)

## Features:
1. All [Core firmware for Universal measurements tool](../../meter_core) features.
2. Supports 3 10pin modules and 2 14pin modules.
3. External I2C device support:
  - 2-channel 1Hz-37MHz frequency counter using PIO module.
  - 2-channel 1Hz-25MHz PWM generator using PIO module.

**Pins:**

Module 1 (14 pin)
- PIN_SDA1/SPI_MOSI 2
- PIN_SCL1/SPI_SCK  3
- PIN_1_0/SPI_MISO  4
- PIN_1_1/SPI_NCS   5
- PIN_1_2/INTERRUPT 0
- PIN_1_3           1

Module 2 (10 pin)
- PIN_SDA2/SPI_MOSI 6
- PIN_SCL2/SPI_SCK  7
- PIN_2_0/SPI_MISO  8
- PIN_2_1/SPI_NCS   9

Module 3 (10 pin)
- PIN_SDA3/SPI_MOSI 12
- PIN_SCL3/SPI_SCK  13
- PIN_3_0/SPI_MISO  14
- PIN_3_1/SPI_NCS   15

Module 4 (10 pin)
- PIN_SDA4/SPI_MOSI 17
- PIN_SCL4/SPI_SCK  16
- PIN_4_0/SPI_MISO  19
- PIN_4_1/SPI_NCS   18

Module 5 (14 pin)
- PIN_SDA5/SPI_MOSI 26
- PIN_SCL5/SPI_SCK  22
- PIN_5_0/SPI_MISO  28
- PIN_5_1/SPI_NCS   27
- PIN_5_2/INTERRUPT 21
- PIN_5_3           20
