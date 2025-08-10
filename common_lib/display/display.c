#include "board.h"
#include <display.h>
#include <spi_lcd_common.h>
#include <string.h>

typedef struct
{
  Character chars[DISPLAY_MAX_ROWS][DISPLAY_MAX_COLUMNS];
  Rectangle rectangles[DISPLAY_MAX_RECTANGLES];
} Display;

static Display display;

void DisplayInit(void)
{
  memset(&display, 0, sizeof(display));
  for (int y = 0; y < DISPLAY_MAX_ROWS; y++)
    for (int x = 0; x < DISPLAY_MAX_COLUMNS; x++)
      display.chars[y][x].c = ' ';
}

void DisplayInitChar(unsigned int column, unsigned int row, Character *ch)
{
  Character *c = &display.chars[row][column];
  c->font = ch->font;
  c->x = ch->x;
  c->y = ch->y;
  c->bkColor = ch->bkColor;
  c->textColor = ch->textColor;
  c->c = ' ';
}

void DisplayInitRectangle(unsigned int idx, Rectangle *rc)
{
  Rectangle *r = &display.rectangles[idx];
  r->x = rc->x;
  r->y = rc->y;
  r->width = rc->width;
  r->height = rc->height;
  r->color = BLACK_COLOR;
}

void DisplaySetRectangleColor(unsigned int idx, unsigned short color)
{
  Rectangle *r = &display.rectangles[idx];
  if (r->color == color)
    return;
  r->color = color;
  LcdRectFill(r->x, r->y, r->width, r->height, color);
}

void DisplaySetChar(unsigned int column, unsigned int row, char ch)
{
  Character *c = &display.chars[row][column];
  if (c->c != ch)
  {
    c->c = ch;
    LcdDrawChar(c->x, c->y, ch, c->font, c->textColor, c->bkColor);
  }
}

void DisplaySetCharWithColor(unsigned int column, unsigned int row, char ch, unsigned short textColor, unsigned short bkColor)
{
  Character *c = &display.chars[row][column];
  if (c->c != ch || c->textColor != textColor || c->bkColor != bkColor)
  {
    c->textColor = textColor;
    c->bkColor = bkColor;
    c->c = ch;
    LcdDrawChar(c->x, c->y, ch, c->font, c->textColor, c->bkColor);
  }
}
