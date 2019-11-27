#ifndef BOARD_H
#define BOARD_H

#define SSD1306_128_64

#include <lcd_ssd1306.h>

#define SSD1306_SWITCHCAPVCC

#define vsprintf_s(a, b, c, d) vsprintf(a, c, d)

#define BUTTON_PRESSED !(GPIOF->IDR & 1) //PF0

#define VCC 3300 // 3.3v
#define RL 100 // mOhm

void user_init(void);
void user_loop(void);

#endif
