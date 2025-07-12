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

#define ST7789_DC_PIN_CLR
#define ST7789_DC_PIN_SET
#define ST7735_RST_PIN_CLR
#define ST7735_RST_PIN_SET
#define ST7789_CS_PIN_CLR
#define ST7789_CS_PIN_SET

//PA5=SPI1_SCK,PA7=SPI1_MOSI
#define SPI_LCD           SPI1
#define SPI_LCD_ENABLE    RCC_APB2ENR_SPI1EN
#define SPI_TIMEOUT       1000000
#define SPI_LCD_SCK_PIN   GPIO_Pin_5
#define SPI_LCD_SCK_PORT  GPIOA
#define SPI_LCD_SCK_AF    GPIO_AF5_SPI1
#define SPI_LCD_MOSI_PIN  GPIO_Pin_7
#define SPI_LCD_MOSI_PORT GPIOA
#define SPI_LCD_MOSI_AF   GPIO_AF5_SPI1

#include <delay_systick.h>

#endif
