#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define PIN_LED 25

#define UART_BAUD_RATE 115200
#define UART_PORT 0
#define PIN_UART_TXD 43
#define PIN_UART_RXD 44

#define PIN_I2C_SDA_OE  1
#define PIN_I2C_SCL_OE  2
#define PIN_I2C_SDA_IN  3
#define PIN_I2C_SCL_IN  5
#define PIN_DEVICE_ID0  6
#define PIN_DEVICE_ID1  7
#define PIN_DEVICE_ID2  8
#define PIN_RESET       9
#define PIN_SCLK        10
#define PIN_SNCS        11
#define PIN_SDI0        12
#define PIN_SDI1        13
#define PIN_SDI2        14
#define PIN_SDO0        15
#define PIN_SDO1        16
#define PIN_SDO2        17

#define SPI_3BIT_CLK_CLR gpio_put(PIN_SCLK, false)
#define SPI_3BIT_CLK_SET gpio_put(PIN_SCLK, true)
#define SPI_3BIT_CS_CLR gpio_put(PIN_SNCS, false)
#define SPI_3BIT_CS_SET gpio_put(PIN_SNCS, true)

#define MAX_DEVICES 5

#include <hardware/gpio.h>
#include "core_main.h"

#endif
