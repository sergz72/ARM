#include "board.h"
#include "ui.h"
#include <stdlib.h>
#include <font5.h>
#include <font24.h>

#define STATE_STARTX 15
#define STATE_SIZE   10

static unsigned int currentX;

static void DrawChar(unsigned int col, unsigned int y, char c, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int swapColors)
{
  char text[2];
  text[0] = c;
  text[1] = 0;
  if (swapColors)
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, bkColor, textColor, NULL);
  else
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}

static void DrawText(unsigned int col, unsigned int y, char *text, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor)
{
  LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}

static void DrawPinState(unsigned int pin)
{
  unsigned int *state = pinState[pin];
  unsigned int state2 = state[2] >> 1;
  LcdRectFill(currentX, 0, STATE_SIZE, STATE_SIZE, RGB(0, state[1] >> 1, 0)); // green
  LcdRectFill(currentX, STATE_SIZE, STATE_SIZE, STATE_SIZE, RGB(state2, state2, 0)); //yellow
  LcdRectFill(currentX, STATE_SIZE*2, STATE_SIZE, STATE_SIZE, RGB(state[0] >> 1, 0, 0)); // red
  currentX += STATE_SIZE;
}

static void DrawState(void)
{
  currentX = STATE_STARTX;
  InputsCommand(DrawPinState);
}

static unsigned char BuildPinValue(unsigned int pin)
{
  unsigned int *state = pinState[pin];
  if (state[2])
    return 2;
  unsigned int s1 = state[1];
  if (s1 && state[0])
    return 2;
  return s1 ? 1 : 0;
}

static char BuildHexSymbol(unsigned int data0, unsigned int data1, unsigned int data2, unsigned int data3)
{
  unsigned char v0 = BuildPinValue(data0);
  if (v0 > 1)
    return '?';
  unsigned char v1 = BuildPinValue(data1);
  if (v1 > 1)
    return '?';
  v1 <<= 1;
  unsigned char v2 = BuildPinValue(data2);
  if (v2 > 1)
    return '?';
  v2 <<= 2;
  unsigned char v3 = BuildPinValue(data3);
  if (v3 > 1)
    return '?';
  v3 <<= 3;
  v0 |= v1 | v2 | v3;
  return v0 > 9 ? 'A' + (v0 - 10) : v0 + '0';
}

static void Draw8BitHex(void)
{
  char v[2];

  v[1] = 0;
  v[0] = BuildHexSymbol(DATA4, DATA5, DATA6, DATA7);
  LcdDrawText(LCD_WIDTH / 2 - courierNew24ptFontInfo.character_max_width, STATE_SIZE*5, v, &courierNew24ptFontInfo, RGB(0, 0, 255), RGB(0, 0, 0), NULL);
  v[0] = BuildHexSymbol(DATA0, DATA1, DATA2, DATA3);
  LcdDrawText(LCD_WIDTH / 2, STATE_SIZE*5, v, &courierNew24ptFontInfo, RGB(0, 0, 255), RGB(0, 0, 0), NULL);
}

static void DrawPinNumber(unsigned int pin)
{
  char pinNumber[2];
  if (pin < 10)
  {
    itoa((int) pin, pinNumber, 10);
    LcdDrawText(currentX, STATE_SIZE*3, pinNumber, &fiveBySevenFontInfo, RGB(0, 0, 255), RGB(0, 0, 0), NULL);
  }
  else
  {
    itoa((int)pin / 10, pinNumber, 10);
    LcdDrawText(currentX, STATE_SIZE*3, pinNumber, &fiveBySevenFontInfo, RGB(0, 0, 255), RGB(0, 0, 0), NULL);
    itoa((int)pin % 10, pinNumber, 10);
    LcdDrawText(currentX, STATE_SIZE*4, pinNumber, &fiveBySevenFontInfo, RGB(0, 0, 255), RGB(0, 0, 0), NULL);
  }
  currentX += STATE_SIZE;
}

static void DrawPinNumbers(void)
{
  currentX = STATE_STARTX;
  InputsCommand(DrawPinNumber);
}

void UI_Init(void)
{
  LcdInit();
  DrawPinNumbers();
}

void Process_Timer_Event()
{
  /*switch (buttonEvent)
  {
  case IDBUTTONB:
    break;
  case IDBUTTONA:
    break;
  case IDBUTTONCENTER:
    break;
  case IDBUTTONTOP:
    break;
  case IDBUTTONBOTTOM:
    break;
  case IDBUTTONLEFT:
    break;
  case IDBUTTONRIGHT:
    break;
  }*/

  DrawState();
  Draw8BitHex();

  LcdUpdate();

  buttonEvent = IDNOEVENT;
}
