#ifndef DISPLAY_H
#define DISPLAY_H

#include <font.h>

typedef struct
{
  const FONT_INFO *font;
  unsigned short x, y;
  unsigned short textColor;
  unsigned short bkColor;
  char c;
} Character;

typedef struct
{
  unsigned short x, y;
  unsigned short width, height;
  unsigned short color;
} Rectangle;

#ifdef __cplusplus
extern "C" {
#endif

void DisplayInit(void);
void DisplayInitChar(unsigned int column, unsigned int row, Character *ch);
void DisplaySetChar(unsigned int column, unsigned int row, char ch);
void DisplaySetRectangleColor(unsigned int idx, unsigned short color);
void DisplayInitRectangle(unsigned int column, unsigned int row, Rectangle *rc);

#ifdef __cplusplus
}
#endif

#endif
