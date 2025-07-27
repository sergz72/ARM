#include "board.h"
#include <spi_lcd_common.h>

void Lcd_SendCommand(unsigned char Reg)
{
  Lcd_WriteBytes(0, &Reg, 1);
}

void Lcd_SendCommands(unsigned char *data, unsigned int len)
{
  Lcd_WriteBytes(0, data, len);
}

void Lcd_SendData(unsigned char *data, unsigned int len)
{
  Lcd_WriteBytes(LCD_FLAG_DC, data, len);
}

void Lcd_CS_High(void)
{
  unsigned char data = 0;
  Lcd_WriteBytes(LCD_FLAG_CS, &data, 1);
}

void WEAK LcdRectFill(unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color)
{
  LcdSetWindow(column, row, column + dx - 1, row + dy - 1);
  Lcd_WriteColor(color, dx * dy);
  Lcd_CS_High();
}

void LcdScreenFill(unsigned int color)
{
  LcdRectFill(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void LcdDrawChar(unsigned int x, unsigned int y, char c, const FONT_INFO *f, unsigned int textColor, unsigned int bkColor)
{
  if (c < f->start_character || c > f->start_character + f->character_count | x + 16 > LCD_WIDTH || y + f->char_height > LCD_HEIGHT)
    return;
  unsigned int char_bytes = f->char_height << 1;
  const unsigned char *char_pointer = f->char_bitmaps + (c - f->start_character) * char_bytes;
  LcdSetWindow(x, y, x + 15, y + f->char_height - 1);
  c =*char_pointer++;
  unsigned int prev_color = c & 0x80 ? textColor : bkColor;
  unsigned int counter = 0;
  while (char_bytes--)
  {
    for (int i = 0; i < 8; i++)
    {
      unsigned int color = c & 0x80 ? textColor : bkColor;
      if (color == prev_color)
        counter++;
      else
      {
        Lcd_WriteColor(prev_color, counter);
        prev_color = color;
        counter = 1;
      }
      c <<= 1;
    }
    c =*char_pointer++;
  }
  if (counter)
    Lcd_WriteColor(prev_color, counter);
  Lcd_CS_High();
}
