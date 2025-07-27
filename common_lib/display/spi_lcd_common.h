#ifndef SPI_LCD_COMMON_H
#define SPI_LCD_COMMON_H

#define LCD_FLAG_DC 0x100
#define LCD_FLAG_CS 0x200

#define BLACK_COLOR   0

#include <font.h>

#ifdef __cplusplus
extern "C" {
#endif

void Lcd_WriteBytes(unsigned int flags, unsigned char *data, unsigned int size); // should be defined in hal.c
void Lcd_WriteColor(unsigned int color, unsigned int count); // should be defined in hal.c

void Lcd_SendCommand(unsigned char Reg);
void Lcd_SendData(unsigned char *data, unsigned int len);

void LcdDrawChar(unsigned int x, unsigned int y, char c, const FONT_INFO *f, unsigned int textColor, unsigned int bkColor);
void LcdInit(unsigned char madctl);
void LcdRectFill(unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color);
void LcdScreenFill(unsigned int color);
void LcdSetWindow(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void Lcd_CS_High(void);

#ifdef __cplusplus
}
#endif

#endif

