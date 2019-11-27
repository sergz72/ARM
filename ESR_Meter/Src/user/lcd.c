#include "board.h"
#include <stdarg.h>
#include <stdio.h>

const unsigned int white_color = RGB(255, 255, 255);
const unsigned int black_color = RGB(0, 0, 0);

DC screenDC;
DC *cursorDC;
DC *topDC;

int LcdIsPointVisible(DC *dc, unsigned int x, unsigned int y)
{
  if (!topDC)
    return 1;

  if (!dc)
    dc = &screenDC;

  if (dc == topDC)
    return 1;

//  x += dc->left;
//  y += dc->top;

  if (x >= topDC->left && x <= topDC->right &&
      y >= topDC->top && y <= topDC->bottom)
    return 0;

  return 1;
}

/*void LcdClearDC(DC *dc)
{
  if (!dc)
    dc = &screenDC;

  LcdRectFill(dc, 0, 0, LcdGetWidth(dc), LcdGetHeight(dc), dc->bkColor);

  dc->cursorX = dc->cursorY = dc->cursor_state;
}*/

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
unsigned int LcdColorTranslate(int R, int G, int B)
{
  return RGB(R, G, B);
}

void LcdRectangle(DC *dc, unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color, unsigned int PenWidth)
{
  int i;

  column -= PenWidth / 2;
  row -= PenWidth / 2;

  for (i = 0; i < PenWidth; i++)
  {
    LcdLineDrawH(dc, column, column + dx - 1, row + i, color);
    LcdLineDrawH(dc, column, column + dx - 1, row + dy - 1 + i, color);
    LcdLineDrawV(dc, column + i, row, row + dy - 1, color);
    LcdLineDrawV(dc, column + dx - 1 + i, row, row + dy - 1, color);
  }
}

unsigned int display_data_buffer[LCD_WIDTH];

unsigned int lcd_prepare_data_buffer(unsigned int line, char *text, const FONT_INFO *f, unsigned int textColor, unsigned int bkColor)
{
  unsigned int width = 0, w, n;
  unsigned int color;
  FONT_CHAR_INFO fci;
  char spacing = f->character_spacing;
  char c;
  unsigned int *buffer = display_data_buffer;
  unsigned char cc;
  const unsigned char *ccc;

  while ((c = *text++) != 0)
  {
    if (!FontGetChar(c, f, &fci))
      continue;
    w = fci.char_width;
    width += w + spacing;
    ccc = f->char_bitmaps + fci.char_offset + line * (w & 7 ? (w >> 3) + 1 : w >> 3);
    n = 0;
    cc = *ccc;
    while (w--)
    {
      if (cc & 0x80)
        color = textColor;
      else
        color = bkColor;

      *buffer++ = color;

      cc <<= 1;
      n++;
      if (n % 8 == 0)
        cc = *(++ccc);
    }

    for (w = 0; w < spacing; w++)
      *buffer++ = bkColor;
  }

  return width;
}

void LcdNewLine(DC *dc)
{
  short enabled;
  
  if (!dc)
    dc = &screenDC;

  enabled = dc->cursor_enabled;

  if (dc->cursor_enabled)
    LcdCursorEnable(dc, 0);

  dc->cursorY += dc->font->char_height;
  if (dc->cursorY == dc->bottom + 1)
  {
    LcdScroll(dc, dc->font->char_height);
    dc->cursorY = 0;
  }
  else
  {
    if (dc->cursorY > dc->bottom + 1)
      dc->cursorY -= LcdGetHeight(dc);
    if (dc->font->char_height + dc->cursorY > dc->bottom + 1)
      LcdScroll(dc, dc->font->char_height + dc->cursorY - LcdGetHeight(dc));
//    else if (scroll_count)
//      LcdScroll(dc, dc->font->char_height);
  }

  if (enabled)
    LcdCursorEnable(dc, 1);
}

void LcdPutChar(DC *dc, char c)
{
  char buffer[2];
  FONT_CHAR_INFO fci;
  short enabled;

  if (!dc)
    dc = &screenDC;

  if (c == '\r')
  {
    enabled = dc->cursor_enabled;

    if (dc->cursor_enabled)
      LcdCursorEnable(dc, 0);
    
    dc->cursorX = 0;
    
    if (enabled)
      LcdCursorEnable(dc, 1);

    return;
  }
  if (c == '\n')
  {
    LcdNewLine(dc);
    return;
  }
  if (c == 8)
  {
    LcdBS(dc);
    return;
  }

  if (!FontGetChar(c, dc->font, &fci))
    return;

  enabled = dc->cursor_enabled;

  if (dc->cursor_enabled)
    LcdCursorEnable(dc, 0);

  buffer[0] = c;
  buffer[1] = 0;

  dc->cursorX += LcdDrawText(dc, dc->cursorX, dc->cursorY, buffer);
  if (dc->cursorX + dc->font->character_max_width > dc->right + 1)
  {
    dc->cursorX = 0;
    LcdNewLine(dc);
  }

  if (enabled)
    LcdCursorEnable(dc, 1);
}

void LcdBS(DC *dc)
{
  short enabled;
  unsigned short dx;

  if (!dc)
    dc = &screenDC;

  enabled = dc->cursor_enabled;
  dx = dc->font->character_max_width + dc->font->character_spacing;

  if (dc->cursor_enabled)
    LcdCursorEnable(dc, 0);

  if (dc->cursorX >= dx)
    dc->cursorX -= dx;
  else
  {
    if (dc->cursorY >= dc->font->char_height)
      dc->cursorY -= dc->font->char_height;
    else
      dc->cursorY = LcdGetHeight(dc) - dc->font->char_height + dc->cursorY;
    dc->cursorX = LcdGetWidth(dc) - (LcdGetWidth(dc) % dx) - dx;
  }

  LcdRectFill(dc, dc->cursorX, dc->cursorY, dc->font->character_max_width, dc->font->char_height, dc->bkColor);

  if (enabled)
    LcdCursorEnable(dc, 1);
}

void LcdCursorEnable(DC *dc, short enable)
{
  if (!dc)
    dc = &screenDC;

  if (dc->cursor_enabled && !enable)
  {
    dc->cursor_enabled = 0;
    if (dc->cursor_state)
    {
      dc->cursor_state = 0;
      LcdRectFill(dc, dc->cursorX, dc->cursorY, dc->font->character_max_width, dc->font->char_height, dc->bkColor);
    }
  }
  else
    dc->cursor_enabled = enable;
}

void LcdCursorToggle(void)
{
  if (!cursorDC)
    return;

  if (cursorDC->cursor_enabled)
  {
    cursorDC->cursor_state ^= 1;
    LcdRectFill(cursorDC, cursorDC->cursorX, cursorDC->cursorY, cursorDC->font->character_max_width, cursorDC->font->char_height,
                cursorDC->cursor_state ? white_color : cursorDC->bkColor);
  }
}

void LcdInitDC(DC *dc, unsigned short left, unsigned short top, unsigned short width, unsigned short height,
               unsigned int textColor, unsigned int bkColor, const FONT_INFO *font)
{
  dc->left = left;
  dc->right = dc->left + width - 1;
  dc->top = top;
  dc->bottom = dc->top + height - 1;
  dc->textColor = textColor;
  dc->bkColor = bkColor;
  dc->font = font;
  dc->cursorX = dc->cursorY = dc->cursor_enabled = dc->cursor_state = 0;
}

void LcdSetFont(DC *dc, const FONT_INFO *font)
{
  if (!font)
    return;

  if (!dc)
    dc = &screenDC;

  dc->font = font;
}

void LcdSetTextColor(DC *dc, unsigned int textColor)
{
  if (!dc)
    dc = &screenDC;

  dc->textColor = textColor;
}

void LcdSetBkColor(DC *dc, unsigned int bkColor)
{
  if (!dc)
    dc = &screenDC;

  dc->bkColor = bkColor;
}

void LcdSetCursorDC(DC *dc)
{
  DC *old_dc = cursorDC;
  cursorDC = NULL;
  if (old_dc && old_dc->cursor_enabled && old_dc->cursor_state)
  {
    old_dc->cursor_state = 0;
    LcdRectFill(old_dc, old_dc->cursorX, old_dc->cursorY, old_dc->font->character_max_width, old_dc->font->char_height, old_dc->bkColor);
  }
  cursorDC = dc;
}

void LcdPutString(DC *dc, const char *s)
{
  char c;

  while ((c = *s) != 0)
  {
    if (c == '\n')
      LcdPutChar(dc, '\r');
    LcdPutChar(dc, *s++);
  }
}

/*
**************************************************************************************************************
*                                        PRINT FORMATTED STRING
*
* Description: This function is used to print formatted string. This function takes variable length arguments
*
* Arguments  : variable length arguments
*
* Returns    : None
*
**************************************************************************************************************
*/

void LcdPrintf(DC *dc, const char *format, ...)
{
  static char buffer[200 + 1];
  va_list vArgs;

  va_start(vArgs, format);
  vsprintf_s(buffer, sizeof(buffer), format, vArgs);
  va_end(vArgs);
  LcdPutString(dc, buffer);
}

unsigned int LcdDrawTextCenter(DC *dc, unsigned int column, unsigned int row, unsigned int width, char *text)
{
  if (!dc)
    dc = &screenDC;

	column += (width - FontGetWidth(text, dc->font)) / 2;
	return LcdDrawText(dc, column, row, text);
}

unsigned int LcdDrawTextRight(DC *dc, unsigned int column, unsigned int row, unsigned int width, char *text)
{
  if (!dc)
    dc = &screenDC;

	column += width - FontGetWidth(text, dc->font);
	return LcdDrawText(dc, column, row, text);
}
