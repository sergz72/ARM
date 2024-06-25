#ifndef _LCD_H_
#define _LCD_H_

#include <font.h>

typedef struct {
  unsigned short left, top, right, bottom;
  unsigned int textColor, bkColor;
  unsigned short cursorX, cursorY, cursor_enabled, cursor_state;
  const FONT_INFO *font;
} DC;

void LcdInit(const FONT_INFO *sysfont);
unsigned int LcdDrawText(DC *dc, unsigned int column, unsigned int row, char *text);
unsigned int LcdDrawTextCenter(DC *dc, unsigned int column, unsigned int row, unsigned int width, char *text);
unsigned int LcdDrawTextRight(DC *dc, unsigned int column, unsigned int row, unsigned int width, char *text);
void LcdClearDC(DC *dc);
void LcdPutChar(DC *dc, char c);
void LcdBS(DC *dc);
void LcdRectFill(DC *dc, unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color);
void LcdRectangle(DC *dc, unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color, unsigned int PenWidth);
void LcdScroll(DC *dc, short count);
void LcdCursorToggle(void);
void LcdCursorEnable(DC *dc, short enable);
void LcdPixelDraw(DC *dc, unsigned int lX, unsigned int lY, unsigned int Value);
void LcdLineDraw(DC *dc, unsigned int X1, unsigned int Y1, unsigned int X2, unsigned int Y2, unsigned int Value, unsigned int PenWidth);
void LcdLineDrawH(DC *dc, unsigned int lX1, unsigned int lX2, unsigned int lY, unsigned int Value);
void LcdLineDrawV(DC *dc, unsigned int lX, unsigned int lY1, unsigned int lY2, unsigned int Value);
unsigned int LcdColorTranslate(int R, int G, int B);
void LcdInitDC(DC *dc, unsigned short left, unsigned short top, unsigned short width, unsigned short height,
               unsigned int textColor, unsigned int bkColor, const FONT_INFO *font);
void LcdSetFont(DC *dc, const FONT_INFO *font);
void LcdSetTextColor(DC *dc, unsigned int textColor);
void LcdSetBkColor(DC *dc, unsigned int bkColor);
void LcdSetCursorDC(DC *dc);
void LcdPutString(DC *dc, const char *s);
void LcdPrintf(DC *dc, const char *format, ...);
unsigned int lcd_prepare_data_buffer(unsigned int line, char *text, const FONT_INFO *f, unsigned int textColor, unsigned int bkColor);
int LcdIsPointVisible(DC *dc, unsigned int x, unsigned int y);

#define LcdGetWidth(dc) ((dc)->right - (dc)->left + 1)
#define LcdGetHeight(dc) ((dc)->bottom - (dc)->top + 1)

extern const unsigned int white_color, black_color;
extern DC screenDC, *cursorDC, *topDC;
extern unsigned int display_data_buffer[];

#endif /*_LCD_H_*/
