#include "logic_analyser.h"
#include "board.h"
#include <stdlib.h>
#include <font5.h>
#include <font24.h>

#define STATE_STARTX 15
#define STATE_SIZE   10

static unsigned int currentX;

static void LADrawPinState(unsigned int pin)
{
  unsigned int *state = pinState[pin];
  unsigned int state2 = state[2] >> 1;
  LcdRectFill(currentX, 0, STATE_SIZE, STATE_SIZE, RGB(0, state[1] >> 1, 0)); // green
  LcdRectFill(currentX, STATE_SIZE, STATE_SIZE, STATE_SIZE, RGB(state2, state2, 0)); //yellow
  LcdRectFill(currentX, STATE_SIZE*2, STATE_SIZE, STATE_SIZE, RGB(state[0] >> 1, 0, 0)); // red
  currentX += STATE_SIZE;
}

static void LADrawState(void)
{
  currentX = STATE_STARTX;
  LAInputsCommand(LADrawPinState);
}

static unsigned char LABuildPinValue(unsigned int pin)
{
  unsigned int *state = pinState[pin];
  if (state[2])
    return 2;
  unsigned int s1 = state[1];
  if (s1 && state[0])
    return 2;
  return s1 ? 1 : 0;
}

static char LABuildHexSymbol(unsigned int data0, unsigned int data1, unsigned int data2, unsigned int data3)
{
  unsigned char v0 = LABuildPinValue(data0);
  if (v0 > 1)
    return '?';
  unsigned char v1 = LABuildPinValue(data1);
  if (v1 > 1)
    return '?';
  v1 <<= 1;
  unsigned char v2 = LABuildPinValue(data2);
  if (v2 > 1)
    return '?';
  v2 <<= 2;
  unsigned char v3 = LABuildPinValue(data3);
  if (v3 > 1)
    return '?';
  v3 <<= 3;
  v0 |= v1 | v2 | v3;
  return v0 > 9 ? 'A' + (v0 - 10) : v0 + '0';
}

static void LADraw8BitHex(void)
{
  char v[2];

  v[1] = 0;
  v[0] = LABuildHexSymbol(DATA4, DATA5, DATA6, DATA7);
  LcdDrawText(LCD_WIDTH / 2 - courierNew24ptFontInfo.character_max_width, STATE_SIZE*5, v, &courierNew24ptFontInfo, RGB(0, 0, 255), RGB(0, 0, 0), NULL);
  v[0] = LABuildHexSymbol(DATA0, DATA1, DATA2, DATA3);
  LcdDrawText(LCD_WIDTH / 2, STATE_SIZE*5, v, &courierNew24ptFontInfo, RGB(0, 0, 255), RGB(0, 0, 0), NULL);
}

static void LADrawPinNumber(unsigned int pin)
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

void LALcdInit(void)
{
  currentX = STATE_STARTX;
  LAInputsCommand(LADrawPinNumber);
}

void LALcdUpdate(void)
{
  LADrawState();
  LADraw8BitHex();
}