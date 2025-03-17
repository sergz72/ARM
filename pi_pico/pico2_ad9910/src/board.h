#ifndef _BOARD_H
#define _BOARD_H

#include <hardware/gpio.h>

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define MAX_SHELL_COMMANDS                 100
#define MAX_SHELL_COMMAND_PARAMETERS       10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE                 20
#define SHELL_HISTORY_ITEM_LENGTH          100

#define AD9910_EXT_PWR_DOWN_PIN 9
#define AD9910_RESET_PIN 10
#define AD9910_PLL_LOCK_PIN 11
#define AD9910_IO_UPDATE_PIN 12

#define AD9910_MAX_CHANNELS 1
#define AD9910_REFCLK_FREQUENCY 40000000
#define AD9910_PLL_FREQUENCY 1000000000
#define AD9910_RESET_SET(channel) gpio_put(AD9910_RESET_PIN, 1)
#define AD9910_RESET_CLR(channel) gpio_put(AD9910_RESET_PIN, 0)
#define AD9910_EXT_PWR_DOWN_SET(channel) gpio_put(AD9910_EXT_PWR_DOWN_PIN, 1)
#define AD9910_EXT_PWR_DOWN_CLR(channel) gpio_put(AD9910_EXT_PWR_DOWN_PIN, 0)
#define AD9910_PLL_LOCK_CHECK(channel) gpio_get(AD9910_PLL_LOCK_PIN)
#define AD9910_IO_UPDATE_SET(channel) gpio_put(AD9910_IO_UPDATE_PIN, 1)
#define AD9910_IO_UPDATE_CLR(channel) gpio_put(AD9910_IO_UPDATE_PIN, 0)

void SystemInit(void);
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

#endif
