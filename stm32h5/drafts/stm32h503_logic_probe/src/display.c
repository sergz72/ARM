#include "board.h"
#include "display.h"
#include <lcd_st7789.h>

typedef struct
{
  char c;
  unsigned short textColor;
  unsigned short bkCcolor;
} Character;

typedef struct
{
  const FONT_INFO *font;
  Character chars[DISPLAY_MAX_ROWS][DISPLAY_MAX_COLUMNS];
} Display;

static Display display;

void DisplayInit(const FONT_INFO *font)
{
  display.font = font;
  for (int y = 0; y < DISPLAY_MAX_ROWS; y++)
    for (int x = 0; x < DISPLAY_MAX_COLUMNS; x++)
      display.chars[y][x].c = ' ';
}

void DisplaySetCharColor(unsigned int column, unsigned int row, unsigned short textColor, unsigned short bkColor)
{
  Character *c = &display.chars[row][column];
  c->textColor = textColor;
  c->bkCcolor = bkColor;
}

void DisplaySetChar(unsigned int column, unsigned int row, char ch)
{
  Character *c = &display.chars[row][column];
  if (c->c != ch)
  {
    c->c = ch;
    unsigned int x = column << 4;
    unsigned int y = row * display.font->char_height;
    LcdDrawChar(x, y, ch, display.font, c->textColor, c->bkCcolor);
  }
}
