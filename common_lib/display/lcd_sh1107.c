#include "board.h"
#include <lcd_sh1107.h>
#include <string.h>

#define SH1107_COLUMNADDR_LO 0
#define SH1107_COLUMNADDR_HI 0x10
#define SH1107_MEMORYMODE_VERTICAL 0x20
#define SH1107_MEMORYMODE_PAGE     0x21
#define SH1107_SETCONTRAST 0x81
#define SH1107_NORMALDISPLAY 0xA6
#define SH1107_INVERTDISPLAY 0xA7
#define SH1107_SETMULTIPLEX 0xA8
#define SH1107_DCDC_CONTROL 0xAD
#define SH1107_DISPLAYOFF 0xAE
#define SH1107_DISPLAYON 0xAF
#define SH1107_SET_PAGE_ADDRESS 0xB0
#define SH1107_SETDISPLAYOFFSET 0xD3
#define SH1107_SETDISPLAYCLOCKDIV 0xD5
#define SH1107_SETPRECHARGE 0xD9
#define SH1107_SETVCOMDETECT 0xDB
#define SH1107_SETSTARTLINE 0xDC

#ifndef SH1107_SEGREMAP_COMMAND
#define SH1107_SEGREMAP_COMMAND SH1107_SEGREMAP
#endif

#ifndef SH1107_COMSCAN_COMMAND
#define SH1107_COMSCAN_COMMAND SH1107_COMSCANDEC
#endif

#ifndef SH1107_CONTRAST
#define SH1107_CONTRAST 0x80
#endif

#ifndef SH1107_MEMORYMODE
#define SH1107_MEMORYMODE SH1107_MEMORYMODE_PAGE
#endif

#ifdef SH1107_ROTATE90
#define SH1107_GET_BUFFER_OFFSET(x, y) (x + (y >> 3U) * LCD_WIDTH)
#define SH1107_GET_BUFFER_BIT(x, y) (y & 7U)
#else
#define SH1107_GET_BUFFER_OFFSET(x, y) ((x >> 3U) + y * LCD_WIDTH/8)
#define SH1107_GET_BUFFER_BIT(x, y) (x & 7U)
#endif

#define BUFFER_SIZE (LCD_WIDTH * LCD_HEIGHT / 8)

unsigned char sh1107_buffer[BUFFER_SIZE];

static const unsigned char init_sequence[] =
{
	SH1107_MEMORYMODE,
	SH1107_SETMULTIPLEX, LCD_HEIGHT - 1,
	SH1107_SEGREMAP_COMMAND,
	SH1107_COMSCAN_COMMAND,
	SH1107_SETCONTRAST, SH1107_CONTRAST,
	SH1107_DISPLAYON
};

unsigned int LcdInit(void)
{
  delayms(50);
	LCD_RST_SET;
	delay(2);

	SH1107_Write(sizeof(init_sequence), 0, init_sequence);

  return 0;
}

void internalDrawPixel(unsigned int lX, unsigned int lY, unsigned int Value)
{
	if (Value == BLACK_COLOR)
		sh1107_buffer[SH1107_GET_BUFFER_OFFSET(lX, lY)] &=  ~(1 << SH1107_GET_BUFFER_BIT(lX, lY));
	else
		sh1107_buffer[SH1107_GET_BUFFER_OFFSET(lX, lY)] |=  (1 << SH1107_GET_BUFFER_BIT(lX, lY));
}

void LcdPixelDraw(unsigned int lX, unsigned int lY, unsigned int Value)
{
	internalDrawPixel(lX, lY, Value);
}

void LcdScreenFill(unsigned int color)
{
	memset(sh1107_buffer, color == BLACK_COLOR ? 0 : 0xFF, BUFFER_SIZE);
}

void LCD_SetContrast(unsigned char contrast)
{
	unsigned char data[2] = {SH1107_SETCONTRAST, contrast};
	SH1107_Write(2, 0, data);
}

void LcdInvertDisplay(int invert)
{
	unsigned char data = invert ? SH1107_INVERTDISPLAY : SH1107_NORMALDISPLAY;
	SH1107_Write(1, 0, &data);
}

void LcdOff(void)
{
	unsigned char data = SH1107_DISPLAYOFF;
	SH1107_Write(1, 0, &data);
}

void LcdOn(void)
{
	unsigned char data = SH1107_DISPLAYON;
	SH1107_Write(1, 0, &data);
}

unsigned int LcdUpdate(void)
{
	int i;
	unsigned char *buffer = sh1107_buffer;
	unsigned char data[3];

#ifdef SH1107_ROTATE90
	data[1] = SH1107_COLUMNADDR_LO;
	data[2] = SH1107_COLUMNADDR_HI;
	for (i = 0; i< LCD_HEIGHT/8; i++)
	{
		data[0] = SH1107_SET_PAGE_ADDRESS + i;
		SH1107_Write(3, 0, data);
		SH1107_Write(LCD_WIDTH, 1, buffer);
		buffer += LCD_WIDTH;
	}
#else
	int row_bytes = LCD_WIDTH/8;
	for (int row = 0; row < LCD_HEIGHT; row++)
	{
		data[0] = row & 0x0F;
		data[1] = SH1107_COLUMNADDR_HI | (row >> 4);
		SH1107_Write(2, 0, data);
		SH1107_Write(row_bytes, 1, buffer);
		buffer += row_bytes;
	}
#endif

	return 0;
}
