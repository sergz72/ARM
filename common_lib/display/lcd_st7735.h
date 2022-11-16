#ifndef LCD_ST7735_H_
#define LCD_ST7735_H_

#include <lcd.h>

#ifdef ST7735_160_80
#define LCD_HWWIDTH  160
#define LCD_HWHEIGHT 80
#define ST7735_XSTART 1
#define ST7735_YSTART 26
#else
  #error "At least one ST7735 display must be defined"
#endif

#if LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE || LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE_REVERSED
  #define LCD_WIDTH  LCD_HWWIDTH
  #define LCD_HEIGHT LCD_HWHEIGHT
#else
  #define LCD_WIDTH  LCD_HWHEIGHT
  #define LCD_HEIGHT LCD_HWWIDTH
#endif

#define BLACK_COLOR   0
#define	BLUE_COLOR    0x1F00
#define	RED_COLOR     0x00F8
#define	GREEN_COLOR   0xE007
#define CYAN_COLOR    0xFF7F
#define MAGENTA_COLOR 0x1FF8
#define YELLOW_COLOR  0xE0FF
#define WHITE_COLOR   0xFFFF
#define GRAY_COLOR    0x3084
#define RGB(r, g, b) ((r & 0xF8) | ((g >> 5) & 7) | ((g & 0x1C) << 11) | ((b & 0xF8) << 5))

void ST7735_WriteBytes(unsigned char *data, unsigned int size);
void ST7735_SET_PWM(unsigned int duty);

#endif
