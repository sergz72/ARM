#ifndef _LCD_HPP
#define _LCD_HPP

#include "board.h"
#include "../font.h"
#include <memory.h>

#define LCD_ORIENTATION_PORTRAIT           1
#define LCD_ORIENTATION_LANDSCAPE          2
#define LCD_ORIENTATION_PORTRAIT_REVERSED  3
#define LCD_ORIENTATION_LANDSCAPE_REVERSED 4

#define BLACK_COLOR 0

template<typename T>
class LCD
{
private:
  void WriteData(unsigned int x, unsigned int y, const unsigned char *bitmap, unsigned int h,
                 unsigned int bit, unsigned int char_bytes, T textColor, T bkColor)
  {
    if (bitmap != nullptr)
      bitmap += bit >> 3U;
    bit =  1U << (7U - (bit % 8U));

    while (h)
    {
      if (bitmap != nullptr && (*bitmap & bit))
        DrawPixel(x, y, textColor);
      else
        DrawPixel(x, y, bkColor);

      h--;
      y++;
      if (bitmap != nullptr)
        bitmap += char_bytes;
    }
  }

  void SetOrientationInit(int _orientation)
  {
    orientation = _orientation;
    if (orientation == LCD_ORIENTATION_LANDSCAPE || orientation == LCD_ORIENTATION_LANDSCAPE_REVERSED)
    {
      width = hwWidth;
      height = hwHeight;
    }
    else
    {
      width = hwHeight;
      height = hwWidth;
    }
  }
protected:
  int orientation;
  unsigned int hwWidth;
  unsigned int hwHeight;
  unsigned int width;
  unsigned int height;
  FONT_CHAR_INFO fci[DRAW_TEXT_MAX];
  T *buffer1;
  T *buffer2;
  unsigned int bufferSize;
  bool firstUpdate;

  virtual unsigned int GetBufferOffset(unsigned int x, unsigned int y) = 0;
  virtual unsigned int InternalUpdate(bool all) = 0;
  virtual void SetHWOrientation() = 0;
public:
  LCD(int _orientation, unsigned int _hwWidth, unsigned int _hwHeight, T *_buffer1, T *_buffer2)
  {
    hwWidth = _hwWidth;
    hwHeight = _hwHeight;
    buffer1 = _buffer1;
    buffer2 = _buffer2;
    bufferSize = _hwWidth * _hwHeight;
    SetOrientationInit(_orientation);
    auto s = bufferSize * sizeof(T);
    // clear screen data
    memset(buffer1, 0, s);
    firstUpdate = true;
  }

  void SetOrientation(int _orientation)
  {
    SetOrientationInit(_orientation);
    SetHWOrientation();
  }

  unsigned int DrawText(unsigned int x, unsigned int y, const char *text, const FONT_INFO *f, T textColor,
                        T bkColor, unsigned int *owidth)
  {
    char c;
    unsigned int col, textWidth, max_width, w, h, bit, character_bytes;
    const unsigned char *bitmap;
    static FONT_CHAR_INFO *pfci;

    if (y >= width || x >= height)
      return 0;

    col = x;
    pfci = fci;
    textWidth = 0;
    max_width = width - col;

    while ((c = *text++) != 0 && max_width)
    {
      if (FontGetChar(c, f, pfci))
      {
        w = pfci->char_width > max_width ? max_width : pfci->char_width;
        col += w;
        textWidth += w;
        max_width -= w;
      }
      pfci++;
      if (max_width)
      {
        w = f->character_spacing > max_width ? max_width : f->character_spacing;
        col += w;
        textWidth += w;
        max_width -= w;
      }
    }

    pfci = fci;
    bitmap = f->char_bitmaps + pfci->char_offset;
    h = y + f->char_height > height ? height - y : f->char_height;
    w = textWidth;
    bit = 0;
    character_bytes = pfci->char_width >> 3U;
    if (pfci->char_width & 7U)
      character_bytes++;

    while (w--)
    {
      WriteData(x++, y, bitmap, h, bit++, character_bytes, textColor, bkColor);
      if (bit >= pfci->char_width)
      {
        if (!w)
          break;
        for (bit = 0; bit < f->character_spacing; bit++)
        {
          WriteData(x++, y, 0, h, 0, 0, textColor, bkColor);
          w--;
          if (!w)
            return textWidth;
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
      *owidth = textWidth;

    return 1;
  }

  void RectFill(unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, T color)
  {
    unsigned int xMax, yMax;
    unsigned int lX, lY;

    xMax = column + dx - 1;
    if (xMax >= width)
      xMax = width - 1;

    yMax = row + dy - 1;
    if (yMax >= height)
      yMax = height - 1;

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
        DrawPixel(lX, lY, color);
      }
    }
  }

  unsigned int DrawTextCenter(unsigned int x, unsigned int y, char* text, const FONT_INFO* f, T textColor,
                              T bkColor, unsigned int w)
  {
    x += (w - FontGetWidth(text, f)) / 2;
    return DrawText(x, y, text, f, textColor, bkColor, NULL);
  }

  unsigned int DrawTextRight(unsigned int x, unsigned int y, char* text, const FONT_INFO* f,
                             T textColor, T bkColor, unsigned int w)
  {
    x += w - FontGetWidth(text, f);
    return DrawText(x, y, text, f, textColor, bkColor, NULL);
  }

  void ScreenFill(T color)
  {
    auto p = buffer1;
    for (int i = 0; i < bufferSize; i++)
      *p++ = color;
  }

  void DrawPixel(unsigned int x, unsigned int y, T color)
  {
    auto p = buffer1 + GetBufferOffset(x, y);
    *p = color;
  }

  unsigned int Update()
  {
    auto rc = InternalUpdate(firstUpdate);
    firstUpdate = false;
    return rc;
  }

  virtual void On() = 0;
  virtual void Off() = 0;

  virtual void Init() = 0;
};

#endif
