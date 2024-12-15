#ifndef OVENCONTROLUI_BOARD_H
#define OVENCONTROLUI_BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define SSD1306_128_64
#define LCD_ORIENTATION LCD_ORIENTATION_LANDSCAPE

#include <lcd_ssd1306.h>

#define LCD_PRINTF_BUFFER_LENGTH 30
#define DRAW_TEXT_MAX 20
#define USE_VSNPRINTF

#define WS2812_PIN 16

#define KEYBOARD_PIN_START 2
#define KEYBOARD_PIN_END   5

#define OVEN_CONTROL_PIN 8

#define KB_UP 0xE
#define KB_DOWN 0xD
#define KB_START 0xB
#define KB_STOP 0x7

#define ZOOM 4

void delayms(unsigned int ms);
void SystemInit(void);
void put_pixel(unsigned int pixel_grb);
unsigned int urgb_u32(unsigned int r, unsigned int g, unsigned int b);
unsigned int get_keyboard_status(void);
void off(void);
void on(void);
void WS2812Init(void);
int get_temperature(void);
int get_lcd_buffer_bit(int x, int y);

#endif
