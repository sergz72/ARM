#include "board.h"
#include "parallel_8bit.h"
#include <font24.h>
#include <font12.h>
#include "ui.h"

void P8LcdInit(void)
{
  LcdUpdate();
}

static void P8DrawPin(unsigned int col, const char *text, unsigned int state)
{
  DrawText(col, 0, text, &courierNew12ptFontInfo, state ? GREEN_COLOR : RED_COLOR, BLACK_COLOR);
}

static char P8BuildHexSymbol(unsigned int data0, unsigned int data1, unsigned int data2, unsigned int data3)
{
  unsigned char v = (p8code >> data0) & 1;
  v |= (p8code >> (data1 - 1)) & 2;
  v |= (p8code >> (data2 - 2)) & 4;
  v |= (p8code >> (data3 - 3)) & 8;
  return v > 9 ? 'A' + (v - 10) : v + '0';
}

static void P8Draw8BitHex(void)
{
  char v[3];

  v[0] = P8BuildHexSymbol(DATA4, DATA5, DATA6, DATA7);
  v[1] = P8BuildHexSymbol(DATA0, DATA1, DATA2, DATA3);
  v[2] = 0;
  LcdDrawText(LCD_WIDTH / 2 - courierNew24ptFontInfo.character_max_width, 50, v, &courierNew24ptFontInfo, BLUE_COLOR, BLACK_COLOR, NULL);
}

void P8LcdUpdate(void)
{
  P8DrawPin(0, "RST", p8code & (1 << P8_RST));
  P8DrawPin(4, "CS", p8code & (1 << P8_CS));
  P8DrawPin(7, "DC", p8code & (1 << P8_DC));
  P8DrawPin(10, "RD", p8code & (1 << P8_RD));
  P8Draw8BitHex();
}