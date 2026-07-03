#ifndef _BOARD_H
#define _BOARD_H

#define MAX_SHELL_COMMANDS 100
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define SPI_INST      spi0
#define SPI_BAUD_RATE 1000
#define SPI_TX_PIN   7
#define SPI_SCK_PIN  6
#define SPI_CS_PIN   5
#define SPI_RX_PIN   4

#define CC1101_GD0_PIN 14
#define CC1101_GD2_PIN 8
#define cc1101_CSN_CLR(d) gpio_put(SPI_CS_PIN, 0)
#define cc1101_CSN_SET(d) gpio_put(SPI_CS_PIN, 1)
#define cc1101_GD0 gpio_get(CC1101_GD0_PIN)
#define cc1101_GD2 gpio_get(CC1101_GD2_PIN)
#define CC1101_TIMEOUT 0xFFFF
#define RF_MODE CC1101_MODE_GFSK_600
#define TX_POWER CC1101_TX_POWER_M30_433
#define CC1101_RXOFF_MODE CC1101_RXOFF_MODE_IDLE
#define CC1101_TXOFF_MODE CC1101_TXOFF_MODE_IDLE

void SystemInit(void);
void delay(unsigned int);

#include <hardware/gpio.h>

#endif
