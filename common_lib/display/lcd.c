#include <board.h>
#include <lcd.h>

unsigned int LcdDrawTextCenter(unsigned int x, unsigned int y, char* text, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int width)
{
  x += (width - FontGetWidth(text, f)) / 2;
  return LcdDrawText(x, y, text, f, textColor, bkColor, NULL);
}

unsigned int LcdDrawTextRight(unsigned int x, unsigned int y, char* text, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int width)
{
  x += width - FontGetWidth(text, f);
  return LcdDrawText(x, y, text, f, textColor, bkColor, NULL);
}

void LcdRectFill(unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color)
{
  unsigned int xMax, yMax;
  int lX, lY;

  xMax = column + dx - 1;
  if (xMax >= LCD_WIDTH)
    xMax = LCD_WIDTH - 1;

  yMax = row + dy - 1;
  if (yMax >= LCD_HEIGHT)
    yMax = LCD_HEIGHT - 1;

  //
  // Loop through the pixels of this filled rectangle.
  //
  for(lY = row; lY <= yMax; lY++)
  {
    for(lX = column; lX <= xMax; lX++)
    {
			//
			// Write the pixel value.
			//
			LcdPixelDraw(lX, lY, color);
		}
  }
}

#ifndef LCD_CUSTOM_DRAW_TEXT
void internalDrawPixel(unsigned int x, unsigned int y, unsigned int textColor);

void lcdWriteData(unsigned int x, unsigned int y, const unsigned char *bitmap, unsigned int height,
                      unsigned int bit, unsigned int char_bytes, unsigned int textColor, unsigned int bkColor)
{
  if (bitmap != NULL)
    bitmap += bit >> 3U;
  bit =  1U << (7U - (bit % 8U));

  while (height)
  {
    if (bitmap != NULL && (*bitmap & bit))
      internalDrawPixel(x, y, textColor);
    else
      internalDrawPixel(x, y, bkColor);

    height--;
    y++;
    bitmap += char_bytes;
  }
}

unsigned int LcdDrawText(unsigned int x, unsigned int y, char *text, const FONT_INFO *f, unsigned int textColor, unsigned int bkColor, unsigned int *owidth)
{
  char c;
  unsigned int col, width, max_width, w, h, bit, character_bytes;
  const unsigned char *bitmap;
  static FONT_CHAR_INFO fci[DRAW_TEXT_MAX], *pfci;

  if (y >= LCD_HEIGHT || x >= LCD_WIDTH)
    return 0;

  col = x;
  pfci = fci;
  width = 0;
  max_width = LCD_WIDTH - col;

  while ((c = *text++) != 0 && max_width)
  {
    if (FontGetChar(c, f, pfci))
    {
      w = pfci->char_width > max_width ? max_width : pfci->char_width;
      col += w;
      width += w;
      max_width -= w;
    }
    pfci++;
    if (max_width)
    {
      w = f->character_spacing > max_width ? max_width : f->character_spacing;
      col += w;
      width += w;
      max_width -= w;
    }
  }

  pfci = fci;
  bitmap = f->char_bitmaps + pfci->char_offset;
  h = y + f->char_height > LCD_HEIGHT ? LCD_HEIGHT - y : f->char_height;
  w = width;
  bit = 0;
  character_bytes = pfci->char_width >> 3U;
  if (pfci->char_width & 7U)
    character_bytes++;

  while (w--)
  {
    lcdWriteData(x++, y, bitmap, h, bit++, character_bytes, textColor, bkColor);
    if (bit >= pfci->char_width)
    {
      if (!w)
        break;
      for (bit = 0; bit < f->character_spacing; bit++)
      {
        lcdWriteData(x++, y, 0, h, 0, 0, textColor, bkColor);
        w--;
        if (!w)
          return width;
      }
      pfci++;
      character_bytes = pfci->char_width >> 3;
      if (pfci->char_width & 7)
        character_bytes++;
      bitmap = f->char_bitmaps + pfci->char_offset;
      bit = 0;
    }
  }

  if (owidth)
    *owidth = width;

  return 1;
}
#endif
