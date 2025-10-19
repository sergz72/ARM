#ifndef LCD_SH1107_H
#define LCD_SH1107_H

#include <lcd.h>

#define WHITE_COLOR 1

#define RGB(r, g, b) r

#ifndef LCD_WIDTH
#define LCD_WIDTH 128
#endif
#ifndef LCD_HEIGHT
#define LCD_HEIGHT 128
#endif

#define SH1107_COMSCANINC 0xC0
#define SH1107_COMSCANDEC 0xC8

#define SH1107_SEGREMAP 0xA0

#ifdef __cplusplus
extern "C" {
#endif

void SH1107_Write(int num_bytes, unsigned char dc, const unsigned char *buffer); // should be defined in hal.c

void LCDSetContrast(unsigned char contrast);
void LcdOff(void);
void LcdOn(void);
void LcdInvertDisplay(int invert);
void LcdPrintf(const char *format, unsigned int column, unsigned int row, const FONT_INFO *f, int white_on_black, ...);

#ifdef __cplusplus
}
#endif

#endif
