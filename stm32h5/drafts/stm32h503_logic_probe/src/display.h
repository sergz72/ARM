#ifndef DISPLAY_H
#define DISPLAY_H

#include <font.h>

#ifdef __cplusplus
extern "C" {
#endif

void DisplayInit(const FONT_INFO *font);
void DisplaySetCharColor(unsigned int column, unsigned int row, unsigned short textColor, unsigned short bkColor);
void DisplaySetChar(unsigned int column, unsigned int row, char ch);

#ifdef __cplusplus
}
#endif

#endif
