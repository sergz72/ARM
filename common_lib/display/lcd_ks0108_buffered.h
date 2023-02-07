#ifndef LCD_KS0108_BUFFERED_H_
#define LCD_KS0108_BUFFERED_H_

#include <font.h>

#if LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE || LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE_REVERSED
  #define LCD_WIDTH  LCD_HWWIDTH
  #define LCD_HEIGHT LCD_HWHEIGHT
#else
  #define LCD_WIDTH  LCD_HWHEIGHT
  #define LCD_HEIGHT LCD_HWWIDTH
#endif

#define WHITE_COLOR   1

void LcdSetY(unsigned int Y, unsigned int crystal_id);
void LcdSetX(unsigned int X, unsigned int crystal_id);
void LcdWriteData(unsigned char data, unsigned int crystal_id);
void LcdOn(void);
void LcdOff(void);
void LcdClearScreen(void);
void LcdLineDrawH(unsigned int lX1, unsigned int lX2, unsigned int lY, int color);
void LcdLineDrawV(unsigned int lX, unsigned int lY1, unsigned int lY2, int color);
void LcdRectangle(unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, int color, unsigned int PenWidth);
void LcdPrintf(const char *format, unsigned int column, unsigned int row, const FONT_INFO *f, int white_on_black, ...);
void LcdDelay(void);
void LcdDelay2(void);
void LcdWait(void);

#endif /*LCD_H_*/
