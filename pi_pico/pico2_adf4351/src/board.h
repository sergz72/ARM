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

#define ADF4351_CE_PIN 20

#define ADF4351_CE_CLR(channel) gpio_put(ADF4351_CE_PIN, 0)
#define ADF4351_CE_SET(channel) gpio_put(ADF4351_CE_PIN, 1)

#define ADF4351_MAX_CHANNELS 1

void SystemInit(void);

#endif
