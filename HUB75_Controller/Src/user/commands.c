#include <board.h>
#include <string.h>
#include <fonts/font5.h>
#include <fonts/font8_2.h>
#include <fonts/font10.h>
#include <fonts/font12.h>

unsigned short int buffer[HUB75_MODULE_WIDTH*HUB75_MODULE_HEIGHT*HUB75_NUM_MODULES];
extern int row_counter;

void clear_screen(void)
{
	memset(buffer, 0, sizeof(buffer));
}

void display_off(void)
{
	row_counter = -1;
	HUB75_OE_SET;
}

void display_on(void)
{
	if (row_counter < 0)
	  row_counter = 0;
}

unsigned short *get_address(unsigned int x, unsigned int y)
{
	return buffer + y * HUB75_MODULE_WIDTH * HUB75_NUM_MODULES + x;
}

void fill(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned short color)
{
	unsigned short *addr;
	unsigned int i;
	
	while (h)
	{
		if (y >= HUB75_MODULE_HEIGHT)
			return;
		addr = get_address(x, y);
		for (i = x; i < x + w; i++)
		{
			if (i >= HUB75_MODULE_WIDTH * HUB75_NUM_MODULES)
				break;
			*addr++ = color;
		}
		y++;
		h--;
	}
}

/*void paint(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned short *data)
{
	unsigned short *addr, *datap;
	unsigned int i;
	
	while (h)
	{
		if (y >= HUB75_MODULE_HEIGHT)
			return;
		addr = get_address(x, y);
		datap = data;
		for (i = x; i < x + w; i++)
		{
			if (i >= HUB75_MODULE_WIDTH * HUB75_NUM_MODULES)
				break;
			*addr++ = *datap++;
		}
		y++;
		h--;
		data += w;
	}
}*/

static void WriteData(unsigned short *address, const unsigned char *bitmap, unsigned int height, unsigned int bit, int char_bytes, unsigned short int color)
{
  if (bitmap != NULL)
  	bitmap += bit >> 3;
	bit =  1 << (7 - (bit % 8));

	while (height)
	{
    if (bitmap != NULL && (*bitmap & bit))
      *address = color;
    else
      *address = 0;
		height--;
		bitmap += char_bytes;
		address += HUB75_MODULE_WIDTH*HUB75_NUM_MODULES;
	}
}

static unsigned short *GetDataAddress(unsigned int row, unsigned int col)
{
	return buffer + row * HUB75_MODULE_WIDTH * HUB75_NUM_MODULES + col;
}

static void LcdDrawText(unsigned int column, unsigned int row, unsigned char text_length, char *text, const FONT_INFO *f, unsigned short int color)
{
  char c;
	unsigned short *address;
	unsigned int col, width, max_width, w, h, bit, character_bytes;
  const unsigned char *bitmap;
  static FONT_CHAR_INFO fci[DRAW_TEXT_MAX], *pfci;

  if (row >= HUB75_MODULE_HEIGHT || column >= HUB75_MODULE_WIDTH*HUB75_NUM_MODULES)
    return;

  col = column;
	pfci = fci;
	width = 0;
	max_width = HUB75_MODULE_WIDTH*HUB75_NUM_MODULES - col;

  while (text_length-- && max_width)
	{
		c = *text++;
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
	h = row + f->char_height > HUB75_MODULE_HEIGHT ? HUB75_MODULE_HEIGHT - row : f->char_height;
	w = width;
	bit = 0;
  character_bytes = pfci->char_width >> 3;
  if (pfci->char_width & 7)
    character_bytes++;
	
	address = GetDataAddress(row, column);
  while (w--)
  {
		WriteData(address++, bitmap, h, bit++, character_bytes, color);
		if (bit >= pfci->char_width)
		{
			if (!w)
				break;
			for (bit = 0; bit < f->character_spacing; bit++)
			{
        WriteData(address++, 0, h, 0, 0, color);
				w--;
        if (!w)
          return;
      }
			pfci++;
      character_bytes = pfci->char_width >> 3;
      if (pfci->char_width & 7)
        character_bytes++;
      bitmap = f->char_bitmaps + pfci->char_offset;
			bit = 0;
		}
  }
}

const FONT_INFO *get_font(unsigned int font_id)
{
	switch (font_id)
	{
		case 8:
			return &fiveBySevenFontInfo;
		case 10:
			return &courierNew8ptFontInfo;
		case 13:
			return &courierNew10ptFontInfo;
		case 15:
			return &courierNew12ptFontInfo;
	}
	
	return NULL;
}

int text(unsigned int x, unsigned int y, unsigned short color, unsigned int font_id, unsigned int text_length, char *txt)
{
	const FONT_INFO *font = get_font(font_id);
	if (!font)
	  return 0;
	
	LcdDrawText(x, y, text_length, txt, font, color);
	
	return 1;
}
