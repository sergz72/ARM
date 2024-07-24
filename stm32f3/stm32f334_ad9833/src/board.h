#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f3xx.h>

#define DEBUG
#define NULL 0

#define PLL_SOURCE_HSE        // External clock (8MHz) used to clock the PLL, and the PLL is used as system clock source
#define HSEBYPASS
#define FLASH_LATENCY_0

/// Master clock frequency, in Hz.
#define BOARD_MCK               12000000

/// Master clock frequency, in Hz.
#define BOARD_PCLK1               12000000
#define BOARD_PCLK2               12000000

#define SYSTICK_MULTIPLIER 12

// clr PB7
#define LED_BLUE_OFF GPIOB->BSRR = 0x00800000
// set PB7
#define LED_BLUE_ON GPIOB->BSRR = 0x0080

// clr PB8
#define LED_ORANGE_OFF GPIOB->BSRR = 0x01000000
// set PB8
#define LED_ORANGE_ON GPIOB->BSRR = 0x0100

// clr PB9
#define LED_GREEN_OFF GPIOB->BSRR = 0x02000000
// set PB9
#define LED_GREEN_ON GPIOB->BSRR = 0x0200

#define SERIAL_NO 2
#define SERIAL_SPEED 115200

#define MAX_SHELL_COMMANDS 20
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 100
#define SERIAL_QUEUE_SIZE 100

#define SPI_DELAY(x) 12
#define SPI_CHECK_MISO(x) 0
//SET PB5
#define SPI_MOSI_SET(x) GPIOB->BSRR = 0x20
//CLR PB5
#define SPI_MOSI_CLR(x) GPIOB->BSRR = 0x200000

#define SPI_CHANNEL_AD  0
#define SPI_CHANNEL_MCP 1

#define AD9833_MCLK_FREQ 25000000

void delay(unsigned int);
void delayms(unsigned int);
void SPI_CS_SET(int channel);
void SPI_CS_CLR(int channel);
void SPI_CLK_SET(int channel);
void SPI_CLK_CLR(int channel);

#endif
