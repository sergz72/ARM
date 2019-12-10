#ifndef _LCD_H
#define _LCD_H

#include <font.h>

#define LCD_ORIENTATION_PORTRAIT           0
#define LCD_ORIENTATION_LANDSCAPE          1
#define LCD_ORIENTATION_PORTRAIT_REVERSED  2
#define LCD_ORIENTATION_LANDSCAPE_REVERSED 3

unsigned int LcdInit(void);
void LcdRectFill(unsigned int x1, unsigned int y1, unsigned int dx, unsigned int dy, unsigned int color);
unsigned int LcdUpdate(void);
unsigned int LcdDrawText(unsigned int x, unsigned int y, char *text, const FONT_INFO *f, unsigned int textColor, unsigned int bkColor, unsigned int *width);
void LcdScreenFill(unsigned int color);

#endif
