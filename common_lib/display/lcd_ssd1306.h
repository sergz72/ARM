#ifndef LCD_SSD1306_H_
#define LCD_SSD1306_H_

#include <lcd.h>

#define WHITE_COLOR 1

#ifdef SSD1306_128_64
#define LCD_HWWIDTH    128
#define LCD_HWHEIGHT   64
#else
#ifdef SSD1306_128_32
  #define LCD_HWWIDTH  128
  #define LCD_HWHEIGHT 32
#else
#ifdef SSD1306_96_16
  #define LCD_HWWIDTH  96
  #define LCD_HWHEIGHT 16
#else
  #error "At least one SSD1306 display must be defined"
#endif
#endif
#endif

#if LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE || LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE_REVERSED
  #define LCD_WIDTH  LCD_HWWIDTH
  #define LCD_HEIGHT LCD_HWHEIGHT
#else
  #define LCD_WIDTH  LCD_HWHEIGHT
  #define LCD_HEIGHT LCD_HWWIDTH
#endif

#define SSD1306_I2C_ADDRESS 0x78

//#define SSD1306_EXTERNALVCC
#define SSD1306_SWITCHCAPVCC

#define RGB(r, g, b) r

#define LCD_MAX_CONTRAST 0x8F

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

int SSD1306_I2C_Write(int num_bytes, unsigned char control_byte, unsigned char *buffer);

void LCD_SetContrast(unsigned char contrast);
void LcdInvertDisplay(int invert);
void LcdPrintf(const char *format, unsigned int column, unsigned int row, const FONT_INFO *f, int white_on_black, ...);

#endif /*LCD_SSD1306_H_*/
