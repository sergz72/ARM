#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f0xx.h>

#define HUB75_NUM_MODULES 1
#define HUB75_MODULE_WIDTH 32
#define HUB75_MODULE_HEIGHT 16
#define HUB75_ROW_COUNTER_MASK 0x7F
#define HUB75_COLOR_BITS 4
#define HUB75_COLOR_MASK 0xF

//PBA0-5
#define HUB75_RGB_SET(v) GPIOA->ODR = v
#define HUB75_R1 1
#define HUB75_G1 2
#define HUB75_B1 4
#define HUB75_R2 8
#define HUB75_G2 0x10
#define HUB75_B2 0x20

//set PB0-2
#define HUB75_ADDR_SET(v) GPIOB->ODR = ((v) | 0x1000) // OE should be set

// clr PB11
#define HUB75_CLK_CLR GPIOB->BSRR = 0x8000000
// set PB11
#define HUB75_CLK_SET GPIOB->BSRR = 0x800

// clr PB10
#define HUB75_LAT_CLR GPIOB->BSRR = 0x4000000
// set PB10
#define HUB75_LAT_SET GPIOB->BSRR = 0x400

// clr PB12
#define HUB75_OE_CLR GPIOB->BSRR = 0x10000000
// set PB12
#define HUB75_OE_SET GPIOB->BSRR = 0x1000

#define DRAW_TEXT_MAX 64 - 8

void user_init(void);
void user_loop(void);
void serial_receive(unsigned char *buf, int len);
void clear_screen(void);
void fill(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned short color);
void display_on(void);
void display_off(void);
//void paint(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned short *data);
int text(unsigned int x, unsigned int y, unsigned short color, unsigned int font_id, unsigned int text_length, char *txt);

#endif
