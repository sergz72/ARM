#ifndef _BOARD_H
#define _BOARD_H

#define MAX_SHELL_COMMANDS 100
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define I2C_TIMEOUT 1000000 // 1s

#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
#define I2C_INST    i2c1
#define I2C_SPEED   100000

#define MAX_COMMAND_LINE_LENGTH 128

void SystemInit(void);

#endif
