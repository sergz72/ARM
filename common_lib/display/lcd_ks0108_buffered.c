#include <lcd_ks0108_buffered.h>
#include "board.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <lcd.h>

#if LCD_ORIENTATION == LCD_ORIENTATION_PORTRAIT
#define GET_BUFFER_ADDRESS(x, y) (paint_buffer + ((LCD_HWHEIGHT - 1 - x) >> 3) * LCD_HWWIDTH + y)
#define GET_BUFFER_OFFSET(x, y) ((7 - x) & 7)
#elif LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE
#define GET_BUFFER_ADDRESS(x, y) (paint_buffer + (y >> 3) * LCD_HWWIDTH + x)
#define GET_BUFFER_OFFSET(x, y) (y & 7)
#elif LCD_ORIENTATION == LCD_ORIENTATION_PORTRAIT_REVERSED
#define GET_BUFFER_ADDRESS(x, y) (paint_buffer + (x >> 3) * LCD_HWWIDTH + (LCD_HWWIDTH - 1 - y))
#define GET_BUFFER_OFFSET(x, y) (x & 7)
#elif LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE_REVERSED
#define GET_BUFFER_ADDRESS(x, y) (paint_buffer + ((LCD_HWHEIGHT - 1 - y) >> 3) * LCD_HWWIDTH + (LCD_HWWIDTH - 1 - x))
#define GET_BUFFER_OFFSET(x, y) ((7 - y) & 7)
#else
#error "Wrong LCD_ORIENTATION"
#endif

#define LCD_CONTROLLER_AREA 64
#define LCD_PAGE_COUNT 8

#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 8)

static unsigned char paint_buffer[BUFFER_SIZE];
#ifndef LCD_TEST
static unsigned char display_buffer[BUFFER_SIZE];
#endif

void LcdSetStartLine(unsigned int line, unsigned int crystal_id);

#ifndef CUSTOM_LCD_DELAY
#ifndef LCD_TEST
void LcdDelay(void)
{
  unsigned int d = LCD_DELAY;
  while (d--);
}

void LcdDelay2(void)
{
  unsigned int d = LCD_DELAY * 10;
  while (d--);
}
#endif
#endif

void LcdClearScreen(void)
{
#ifndef LCD_TEST
  unsigned int x, y;
#endif

	memset(paint_buffer, 0, sizeof(paint_buffer));
#ifndef LCD_TEST
  memset(display_buffer, 0, sizeof(display_buffer));

  for(x = 0; x < LCD_PAGE_COUNT; x++)
  {
    LcdSetX(x, LCD_CS1|LCD_CS2);
    LcdSetY(0, LCD_CS1|LCD_CS2);

    for(y = 0; y < LCD_CONTROLLER_AREA; y++)
      LcdWriteData(0, LCD_CS1|LCD_CS2);   
  }
#endif
}

#ifdef LCD_TEST
HBRUSH hWhiteBrush, hBlackBrush;
#endif

unsigned int LcdInit(void)
{
#ifndef LCD_TEST
  LCD_INIT;
  LcdDelay2();
  LCD_CONTROL_SET(LCD_RST);
#else
  hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
  hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
#endif

  LcdClearScreen();

#ifndef LCD_TEST
  LcdSetStartLine(0, LCD_CS1|LCD_CS2);
#endif
  return 1;
}

#ifndef LCD_TEST
#ifndef CUSTOM_LCD_DELAY
void LcdWait(void)
{
  unsigned int d = LCD_WAIT;
  while (d--);
}
#endif

void LcdCommand(unsigned char cmd, unsigned int crystal_id)
{
  LcdWait();
  LCD_CONTROL_SET(crystal_id);
  LCD_CONTROL_CLR(LCD_DI|LCD_RW);
  LCD_DATA_SET(cmd);
  LCD_CONTROL_SET(LCD_E);
  LcdDelay();
  LCD_CONTROL_CLR(LCD_E);
  LcdDelay();
  LCD_DATA_OFF;
  LCD_CONTROL_CLR(crystal_id);
}

void LcdWriteData(unsigned char data, unsigned int crystal_id)
{
  LcdWait();
  LCD_CONTROL_SET(LCD_DI|crystal_id);
  LCD_CONTROL_CLR(LCD_RW);
  LCD_DATA_SET(data);
  LCD_CONTROL_SET(LCD_E);
  LcdDelay();
  LCD_CONTROL_CLR(LCD_E);
  LcdDelay();
  LCD_DATA_OFF;
  LCD_CONTROL_CLR(crystal_id);
}

void LcdOn(void)
{
  LcdCommand(0x3F, LCD_CS1|LCD_CS2);
}

void LcdOff(void)
{
  LcdCommand(0x3E, LCD_CS1|LCD_CS2);
}

void LcdSetY(unsigned int Y, unsigned int crystal_id)
{
  LcdCommand(0x40+(Y&63), crystal_id);
}

void LcdSetX(unsigned int X, unsigned int crystal_id)
{
  LcdCommand(0xB8+(X&7), crystal_id);
}

void LcdSetStartLine(unsigned int line, unsigned int crystal_id)
{
  LcdCommand(0xC0+(line&63), crystal_id);
}
#endif

unsigned char *GetDataAddress(unsigned int row, unsigned int col)
{
  return GET_BUFFER_ADDRESS(col, row);
}

void internalDrawPixel(unsigned int x, unsigned int y, unsigned int textColor)
{
  unsigned int offset = GET_BUFFER_OFFSET(x, y);
  unsigned char *address = GetDataAddress(y, x);
  if (textColor)
    *address &= ~(1 << offset);
  else
    *address |= 1 << offset;
}

void LcdPixelDraw(unsigned int lX, unsigned int lY, unsigned int Value)
{
  internalDrawPixel(lX, lY, Value);
}

unsigned int LcdUpdate(void)
{
#ifndef LCD_TEST
  unsigned int x, y;
	unsigned char *data, *data2, sety, setx;
	
  data = paint_buffer;
  data2 = display_buffer;
  for(x = 0; x < LCD_PAGE_COUNT; x++)
  {
    setx = sety = 1;
    for(y = 0; y < LCD_CONTROLLER_AREA; y++)
    {
      if (*data != *data2)
      {
        if (sety)
        {
          if (setx)
          {
            LcdSetX(x, LCD_CS1);
            setx = 0;
          }
          LcdSetY(y, LCD_CS1);
          sety = 0;
        }
        LcdWriteData(*data, LCD_CS1);
      }
      else
        sety = 1;
      data++;
      data2++;
    }

    setx = sety = 1;
    for(y = 0; y < LCD_CONTROLLER_AREA; y++)
    {
      if (*data != *data2)
      {
        if (sety)
        {
          if (setx)
          {
            LcdSetX(x, LCD_CS2);
            setx = 0;
          }
          LcdSetY(y, LCD_CS2);
          sety = 0;
        }
        LcdWriteData(*data, LCD_CS2);
      }
      else
        sety = 1;
      data++;
      data2++;
    }
  }

	memcpy(display_buffer, paint_buffer, sizeof(paint_buffer));
#else
  InvalidateRect(hWndLCD, NULL, FALSE);
#endif

  return 1;
}

#ifdef LCD_TEST
void LcdPaint(HDC hdc)
{
  int x, y;
  int line = 0;
  unsigned char data, bit;
  RECT r;

  for (y = 0; y < LCD_HEIGHT; y++)
  {
    bit = 1 << (line & 7);
    r.top = y * LCD_SCALE;
    r.bottom = r.top + LCD_SCALE;
    for (x = 0; x < LCD_WIDTH; x++)
    {
      data = paint_buffer[x + LCD_WIDTH * (line / 8)];
      r.left = x * LCD_SCALE;
      r.right = r.left + LCD_SCALE;
      if (data & bit)
        FillRect(hdc, &r, hBlackBrush);
      else
        FillRect(hdc, &r, hWhiteBrush);
    }
    line++;
    if (line >= LCD_HEIGHT)
      line = 0;
  }
}
#endif

void LcdLineDrawH(unsigned int lX1, unsigned int lX2, unsigned int lY, int color)
{
  while (lX1 <= lX2)
    internalDrawPixel(lX1++, lY, color);
}

void LcdLineDrawV(unsigned int lX, unsigned int lY1, unsigned int lY2, int color)
{
  while (lY1 <= lY2)
    internalDrawPixel(lX, lY1++, color);
}

void LcdRectangle(unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, int color, unsigned int PenWidth)
{
  unsigned int i;

  column -= PenWidth / 2;
  row -= PenWidth / 2;

  for (i = 0; i < PenWidth; i++)
  {
    LcdLineDrawH(column, column + dx - 1, row + i, color);
    LcdLineDrawH(column, column + dx - 1, row + dy - 1 + i, color);
    LcdLineDrawV(column + i, row, row + dy - 1, color);
    LcdLineDrawV(column + dx - 1 + i, row, row + dy - 1, color);
  }
}

void LcdPrintf(const char *format, unsigned int column, unsigned int row, const FONT_INFO *f, int white_on_black, ...)
{
  static char buffer[LCD_PRINTF_BUFFER_LENGTH];
  va_list vArgs;
  int textColor, bkColor;

  va_start(vArgs, white_on_black);
#ifdef USE_VSNPRINTF
  vsnprintf(buffer, sizeof(buffer), format, vArgs);
#else
  vsprintf_s(buffer, sizeof(buffer), format, vArgs);
#endif
  va_end(vArgs);
  textColor = white_on_black ? WHITE_COLOR : BLACK_COLOR;
  bkColor = white_on_black ? BLACK_COLOR : WHITE_COLOR;
  LcdDrawText(column, row, buffer, f, textColor, bkColor, NULL);
}
