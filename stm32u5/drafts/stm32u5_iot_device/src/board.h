#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32u5xx.h"

#include <gpio.h>
#include <delay_systick.h>

#define SYSTICK_MULTIPLIER 15
#define SYSTICK_DIVIDER    4

#define SPI_SOFT

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define SPI_PORT_ENABLE    RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOBEN;
#define SPI_TIMEOUT        100000
#define SPI_SCK_PIN        GPIO_Pin_13
#define SPI_SCK_PORT       GPIOB
#define SPI_MOSI_PIN       GPIO_Pin_15
#define SPI_MOSI_PORT      GPIOB
#define SPI_MISO_PIN       GPIO_Pin_14
#define SPI_MISO_PORT      GPIOB
#ifdef SPI_SOFT
#define SPI_CHANNELS       1
#define SPI_DELAY(ch)      30
#define SPI_CLK_ACTIVE(ch) SPI_SCK_PORT->BSRR = SPI_SCK_PIN
#define SPI_CLK_IDLE(ch)   SPI_SCK_PORT->BSRR = SPI_SCK_PIN << 16
#define SPI_CS_CLR(ch)     CC1101_CS_PORT->BSRR = CC1101_CS_PIN << 16
#define SPI_CS_SET(ch)     CC1101_CS_PORT->BSRR = CC1101_CS_PIN
#define SPI_MOSI_CLR(ch)   SPI_MOSI_PORT->BSRR = SPI_MOSI_PIN << 16
#define SPI_MOSI_SET(ch)   SPI_MOSI_PORT->BSRR = SPI_MOSI_PIN
#define SPI_CHECK_MISO(ch) (SPI_MISO_PORT->IDR & SPI_MISO_PIN)
#else
// msik as clock
#define SPI_CLOCK_SELECT   RCC->CCIPR1 |= RCC_CCIPR1_SPI2SEL_0 | RCC_CCIPR1_SPI2SEL_1
#define SPI_INST           SPI2
#define SPI_ENABLE         RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN
#define SPI_AF             GPIO_AF5_SPI2
#endif

#define CC1101_PORT_ENABLE RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOBEN | RCC_AHB2ENR1_GPIOAEN;
#define CC1101_GD0_PORT    GPIOA
#define CC1101_GD0_PIN     GPIO_Pin_8
#define CC1101_GD2_PORT    GPIOA
#define CC1101_GD2_PIN     GPIO_Pin_10
#define CC1101_CS_PIN      GPIO_Pin_12
#define CC1101_CS_PORT     GPIOB
#ifdef SPI_SOFT
#define cc1101_CSN_CLR(d)  CC1101_CS_PORT->BSRR = CC1101_CS_PIN
#define cc1101_CSN_SET(d)  CC1101_CS_PORT->BSRR = CC1101_CS_PIN << 16
#else
#define cc1101_CSN_CLR(d)
#define cc1101_CSN_SET(d)
#endif
#define cc1101_GD0        (CC1101_GD0_PORT->IDR & CC1101_GD0_PIN)
#define cc1101_GD2        (CC1101_GD2_PORT->IDR & CC1101_GD2_PIN)
#define CC1101_TIMEOUT    0xFFFFF
#define RF_MODE           CC1101_MODE_GFSK_600
#define TX_POWER          CC1101_TX_POWER_M30_433
#define CC1101_RXOFF_MODE CC1101_RXOFF_MODE_IDLE
#define CC1101_TXOFF_MODE CC1101_TXOFF_MODE_IDLE

// msik as clock
#define I2C_CLOCK_SELECT RCC->CCIPR1 |= RCC_CCIPR1_I2C1SEL_0 | RCC_CCIPR1_I2C1SEL_1
#define I2C_ENABLE       RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN
#define I2C_AF           GPIO_AF4_I2C1
#define I2C_TIMINGS      0x00000E14
#define I2C_INST         I2C1
#define I2C_PORT_ENABLE  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOBEN;
#define I2C_TIMEOUT      100000
#define SCL_PORT         GPIOB
#define SCL_PIN          GPIO_Pin_8
#define SDA_PORT         GPIOB
#define SDA_PIN          GPIO_Pin_9

#define PRINTF_BUFFER_LENGTH 100
#define SERIAL_QUEUE_SIZE 100

#define VBUS_MIN 2400

#include "usb_func.h"
#include <fixed_queue.h>
#include <delay_systick.h>

extern Queue usb_q;

void USB_Deinit(void);
void USB_Init(void);
unsigned int adc_getvbus(void);
unsigned int adc_getvbat(void);

#endif
