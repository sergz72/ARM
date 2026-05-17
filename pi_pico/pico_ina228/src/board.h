#ifndef _BOARD_H
#define _BOARD_H

#include <hardware/gpio.h>

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define MAX_SHELL_COMMANDS 100
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
#define I2C_TIMEOUT 1000000 // 1s
#define I2C_INST i2c1

#define INA228_MAX_CHANNELS 1

#define INA_ADDRESS 0x40

void delayms(unsigned int ms);
void delay(unsigned int us);
void SystemInit(void);

#endif
