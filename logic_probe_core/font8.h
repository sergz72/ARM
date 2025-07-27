#ifndef FONT8_H
#define FONT8_H

//"0123456789%HLUVZ "
#define CHAR_PERCENT 20
#define CHAR_H       21
#define CHAR_L       22
#define CHAR_U       23
#define CHAR_V       24
#define CHAR_Z       25
#define CHAR_SPACE   26

typedef void (*draw_symbol)(unsigned int text_color, unsigned int bk_color);

extern const draw_symbol font8_symbols[27];

#endif //FONT8_H
