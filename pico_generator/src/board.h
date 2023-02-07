#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define ST7735_160_80
#define LCD_ORIENTATION LCD_ORIENTATION_PORTRAIT

#include <lcd_st7735.h>
#include <hardware/gpio.h>

#define ST7735_RST_PIN  12
#define ST7735_DC_PIN   8
#define ST7735_CS_PIN   9
#define ST7735_BL_PIN   13
#define ST7735_CLK_PIN  10
#define ST7735_MOSI_PIN 11

#define PWM1_PIN 19
#define PWM2_PIN 26

#define ST7735_DC_PIN_CLR  gpio_put(ST7735_DC_PIN, 0)
#define ST7735_DC_PIN_SET  gpio_put(ST7735_DC_PIN, 1)
#define ST7735_CS_PIN_CLR  gpio_put(ST7735_CS_PIN, 0)
#define ST7735_CS_PIN_SET  gpio_put(ST7735_CS_PIN, 1)
#define ST7735_RST_PIN_CLR gpio_put(ST7735_RST_PIN, 0)
#define ST7735_RST_PIN_SET gpio_put(ST7735_RST_PIN, 1)
#define ST7735_SPI_PORT    spi1

#define DRAW_TEXT_MAX 10

#define TIMER_INTERVAL 250

#define FMAX 9999
#define FMIN 1
#define DMIN 1
#define DMAX 9999
#define AMIN 1
#define AMAX 329

#define FALSE 0
#define TRUE  1

extern volatile int buttonEvent;

void delayms(unsigned int ms);
void SystemInit(void);
void enableOutput(unsigned int mode, unsigned int channel, unsigned int enabled);
void setParams(unsigned int channel, unsigned int mode, unsigned int frequency, unsigned int duty, unsigned int amplitude);

#endif
