#ifndef FONT_H
#define FONT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned char char_width;
  unsigned short int char_offset;
} FONT_CHAR_INFO;

typedef struct {
  unsigned char char_height;
  char start_character;
  unsigned char character_count;
  unsigned char character_max_width;
  unsigned char character_spacing;
  const FONT_CHAR_INFO *char_info;
  const unsigned char *char_bitmaps;
} FONT_INFO;

int FontGetChar(char c, const FONT_INFO *f, FONT_CHAR_INFO *font_char_info);
unsigned int FontGetWidth(const char *string, const FONT_INFO *f);

#ifdef __cplusplus
}
#endif

#endif

