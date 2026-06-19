#ifndef _BOARD_H
#define _BOARD_H

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

#define I2C_TIMEOUT 1000000 // 1s

#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define I2C_INST    i2c1
#define I2C_SPEED   100000

#define MAX_COMMAND_LINE_LENGTH 128

#define BH1750_ADDR        0x23

void delayms(unsigned int ms);
void delay(unsigned int us);
void SystemInit(void);

#endif
