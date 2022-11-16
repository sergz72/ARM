#include <font.h>

int FontGetChar(char c, const FONT_INFO *f, FONT_CHAR_INFO *font_char_info)
{
  char start = f->start_character;
  unsigned short character_bytes;

  if (c < start || c >= f->start_character + f->character_count)
	{
		font_char_info->char_width = 0;
    return 0;
	}

  c -= start;

  if (f->char_info)
	{
		font_char_info->char_offset = f->char_info[c].char_offset;
		font_char_info->char_width = f->char_info[c].char_width;
    return 1;
	}

  character_bytes = f->character_max_width >> 3;
  if (f->character_max_width & 7)
    character_bytes++;

  font_char_info->char_width = f->character_max_width;
  font_char_info->char_offset = character_bytes * f->char_height * c;

  return 1;
}

unsigned int FontGetWidth(const char *string, const FONT_INFO *f)
{
	FONT_CHAR_INFO info;
	unsigned int width = 0;
	
	while (*string)
	{
		if (FontGetChar(*string++, f, &info))
			width += info.char_width;
		width += f->character_spacing;
	}
	
	return width;
}
