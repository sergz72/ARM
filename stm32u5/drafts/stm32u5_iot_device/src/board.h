#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32u5xx.h"

#include <gpio.h>
#include <delay_systick.h>

#define SYSTICK_MULTIPLIER 1
#define SYSTICK_DIVIDER    2

// clr PC7
#define LED_GREEN_OFF GPIOC->BSRR = 0x800000
// set PC7
#define LED_GREEN_ON GPIOC->BSRR = 0x80

// clr PB7
#define LED_BLUE_OFF GPIOB->BSRR = 0x800000
// set PB7
#define LED_BLUE_ON GPIOB->BSRR = 0x80

// clr PG2
#define LED_RED_OFF GPIOG->BSRR = 0x40000
// set PG2
#define LED_RED_ON GPIOG->BSRR = 4

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200
#define USART_BUFFER_SIZE 1024

#define SPI_CHANNELS       1
#define SPI_DELAY(ch)      4
#define SPI_PORT_ENABLE    RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN;
#define SPI_TIMEOUT        100000
#define SPI_SCK_PIN        GPIO_Pin_5
#define SPI_SCK_PORT       GPIOA
#define SPI_MOSI_PIN       GPIO_Pin_7
#define SPI_MOSI_PORT      GPIOA
#define SPI_MISO_PIN       GPIO_Pin_6
#define SPI_MISO_PORT      GPIOA
#define SPI_CLK_ACTIVE(ch) SPI_SCK_PORT->BSRR = SPI_SCK_PIN
#define SPI_CLK_IDLE(ch)   SPI_SCK_PORT->BSRR = SPI_SCK_PIN << 16
#define SPI_CS_CLR(ch)     CC1101_CS_PORT->BSRR = CC1101_CS_PIN << 16
#define SPI_CS_SET(ch)     CC1101_CS_PORT->BSRR = CC1101_CS_PIN
#define SPI_MOSI_CLR(ch)   SPI_MOSI_PORT->BSRR = SPI_MOSI_PIN << 16
#define SPI_MOSI_SET(ch)   SPI_MOSI_PORT->BSRR = SPI_MOSI_PIN
#define SPI_CHECK_MISO(ch) (SPI_MISO_PORT->IDR & SPI_MISO_PIN)

#define CC1101_GD0_PORT   GPIOC
#define CC1101_GD0_PIN    GPIO_Pin_8
#define CC1101_GD2_PORT   GPIOC
#define CC1101_GD2_PIN    GPIO_Pin_9
#define CC1101_CS_PIN     GPIO_Pin_10
#define CC1101_CS_PORT    GPIOC
#define cc1101_CSN_CLR(d)
#define cc1101_CSN_SET(d)
#define cc1101_GD0        (CC1101_GD0_PORT->IDR & CC1101_GD0_PIN)
#define cc1101_GD2        (CC1101_GD2_PORT->IDR & CC1101_GD2_PIN)
#define CC1101_TIMEOUT    0xFFFFF
#define RF_MODE           CC1101_MODE_GFSK_600
#define TX_POWER          CC1101_TX_POWER_M30_433
#define CC1101_RXOFF_MODE CC1101_RXOFF_MODE_IDLE
#define CC1101_TXOFF_MODE CC1101_TXOFF_MODE_IDLE

#define I2C_SOFT
#define i2c_dly delay(5)

#define I2C_PORT_ENABLE RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOBEN;
#define I2C_TIMEOUT     100000
#define SCL_PORT        GPIOB
#define SCL_PIN         GPIO_Pin_8
#define SDA_PORT        GPIOB
#define SDA_PIN         GPIO_Pin_9

#define SCL_HIGH(ch) SCL_PORT->BSRR = SCL_PIN
#define SCL_LOW(ch)  SCL_PORT->BSRR = SCL_PIN << 16
#define SCL_IN(ch)   (SCL_PORT->IDR & SCL_PIN)
#define SDA_HIGH(ch) SDA_PORT->BSRR = SDA_PIN
#define SDA_LOW(ch)  SDA_PORT->BSRR = SDA_PIN << 16
#define SDA_IN(ch)   (SDA_PORT->IDR & SDA_PIN)

void puts_(const char *s);
int getch_(void);
void usart_start(void);

#endif
