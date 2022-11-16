#ifndef LCD_SSD1306_H_
#define LCD_SSD1306_H_

#include <lcd.h>

#define BLACK_COLOR 0
#define WHITE_COLOR 1

#if !defined SSD1306_128_64 && !defined SSD1306_128_32 && !defined SSD1306_96_16
  #error "At least one SSD1306 display must be defined"
#endif

#if defined SSD1306_128_64
  #define LCD_WIDTH                  128
  #define LCD_HEIGHT                 64
#endif
#if defined SSD1306_128_32
  #define LCD_WIDTH                  128
  #define LCD_HEIGHT                 32
#endif
#if defined SSD1306_96_16
  #define LCD_WIDTH                  96
  #define LCD_HEIGHT                 16
#endif

#define SSD1306_I2C_ADDRESS 0x78

//#define SSD1306_EXTERNALVCC
#define SSD1306_SWITCHCAPVCC

#define RGB(r, g, b) r

#define LCD_MAX_CONTRAST 0x8F

int SSD1306_I2C_Write(int num_bytes, unsigned char control_byte, unsigned char *buffer);

void LCD_SetContrast(unsigned char contrast);
void LCD_Update(void);
void LcdInvertDisplay(int invert);

#endif /*LCD_SSD1306_H_*/
