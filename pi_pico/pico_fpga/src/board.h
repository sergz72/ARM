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
#define PIN_UART_TXD 16
#define PIN_UART_RXD 17

#define PIN_I2C_SDA_OE  14
#define PIN_I2C_SCL_OE  15
#define PIN_I2C_SDA_IN  12
#define PIN_I2C_SCL_IN  13
#define PIN_DEVICE_ID0  3
#define PIN_DEVICE_ID1  4
#define PIN_DEVICE_ID2  5
#define PIN_RESET       22
#define PIN_SCLK        10
#define PIN_SNCS        11
#define PIN_SDI0        8
#define PIN_SDI1        9
#define PIN_SDO0        6
#define PIN_SDO1        7
#define PIN_INTERRUPT   2

#define PIN_SDA4 27
#define PIN_SCL4 26
#define PIN_4_0  28

#define PIN_SDA5 19
#define PIN_SCL5 18
#define PIN_5_0  20
#define PINB_5_1 21

#define SPI_2BIT_CLK_CLR gpio_put(PIN_SCLK, false)
#define SPI_2BIT_CLK_SET gpio_put(PIN_SCLK, true)
#define SPI_2BIT_CS_CLR gpio_put(PIN_SNCS, false)
#define SPI_2BIT_CS_SET gpio_put(PIN_SNCS, true)

void SPI_2BIT_DATA_SET(unsigned int data);
unsigned int spi2_data_get(void);
#define SPI_2BIT_DATA_GET spi2_data_get()

#define MAX_DEVICES 6

#include <hardware/gpio.h>
#include "core_main.h"

#endif
