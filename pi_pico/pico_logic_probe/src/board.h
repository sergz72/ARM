#ifndef _BOARD_H
#define _BOARD_H

#define NULL 0

#define ST7735_160_80
#define LCD_ORIENTATION LCD_ORIENTATION_LANDSCAPE

#include <lcd_st7735.h>
#include <hardware/gpio.h>

#define ST7735_DC_PIN   8
#define ST7735_CS_PIN   9
#define ST7735_CLK_PIN  10
#define ST7735_MOSI_PIN 11
#define ST7735_RST_PIN  12
#define ST7735_BL_PIN   13

#define KEY_UP     2
#define KEY_CENTER 3
#define KEY_A      15
#define KEY_LEFT   16
#define KEY_B      17
#define	KEY_DOWN   18
#define KEY_RIGHT  20
#define MAX_KEY    7

#define ST7735_DC_PIN_CLR  gpio_put(ST7735_DC_PIN, 0)
#define ST7735_DC_PIN_SET  gpio_put(ST7735_DC_PIN, 1)
#define ST7735_CS_PIN_CLR  gpio_put(ST7735_CS_PIN, 0)
#define ST7735_CS_PIN_SET  gpio_put(ST7735_CS_PIN, 1)
#define ST7735_RST_PIN_CLR gpio_put(ST7735_RST_PIN, 0)
#define ST7735_RST_PIN_SET gpio_put(ST7735_RST_PIN, 1)
#define ST7735_SPI_PORT    spi1

#define DATA0 0
#define DATA1 1
#define DATA2 4
#define DATA3 5
#define DATA4 6
#define DATA5 7
#define DATA6 14
#define DATA7 19
#define DATA8  21
#define DATA9  22
#define DATA10 26
#define DATA11 27
#define DATA12 28

#define DRAW_TEXT_MAX 5

#define FALSE 0
#define TRUE  1

extern volatile int buttonEvent;

void delayms(unsigned int ms);
void SystemInit(void);

#endif
