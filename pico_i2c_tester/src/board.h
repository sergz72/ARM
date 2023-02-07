#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define MAX_SHELL_COMMANDS 20
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define SPI3_CLK_PIN 0
#define SPI3_DIO_PIN 1
#define SPI3_CS_PIN  2

#include <hardware/gpio.h>

#define SPI3_DIO_SET gpio_set_dir(SPI3_DIO_PIN, false)
#define SPI3_DIO_CLR gpio_set_dir(SPI3_DIO_PIN, true)
#define SPI3_CLK_SET gpio_put(SPI3_CLK_PIN, 1)
#define SPI3_CLK_CLR gpio_put(SPI3_CLK_PIN, 0)
#define SPI3_CHECK_DIO gpio_get(SPI3_DIO_PIN)
#define SPI3_CS_SET gpio_put(SPI3_CS_PIN, 1)
#define SPI3_CS_CLR gpio_put(SPI3_CS_PIN, 0)
#define SPI3_CUSTOM_DELAY
#define SPI3_LSB_FIRST

void SystemInit(void);

#endif