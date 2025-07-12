#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32h5xx.h"

#define SYSTICK_MULTIPLIER 30
#define SPI_CLOCK 48000000

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

//PA0
#define BUTTON_PRESSED (GPIOA->IDR & 1)

//PB1
#define ST7789_DC_PIN_CLR GPIOB->BSRR = 0x20000
#define ST7789_DC_PIN_SET GPIOB->BSRR = 2
//PB0
#define ST7789_RST_PIN_CLR GPIOB->BSRR = 0x10000
#define ST7789_RST_PIN_SET GPIOB->BSRR = 1
//PB2
#define ST7789_CS_PIN_CLR GPIOB->BSRR = 0x40000
#define ST7789_CS_PIN_SET GPIOB->BSRR = 4

//PA5=SPI1_SCK,PA7=SPI1_MOSI
#define SPI_LCD           SPI1
#define SPI_LCD_ENABLE    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN
#define SPI_TIMEOUT       1000000
#define SPI_LCD_SCK_PIN   GPIO_Pin_5
#define SPI_LCD_SCK_PORT  GPIOA
#define SPI_LCD_SCK_AF    GPIO_AF5_SPI1
#define SPI_LCD_MOSI_PIN  GPIO_Pin_7
#define SPI_LCD_MOSI_PORT GPIOA
#define SPI_LCD_MOSI_AF   GPIO_AF5_SPI1

#define LCD_WIDTH  240
#define LCD_HEIGHT 135
#define ST7789_MADCTL_VALUE (ST7789_MADCTL_MX | ST7789_MADCTL_MV)

#include <delay_systick.h>

#endif
