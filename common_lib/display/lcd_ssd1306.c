#include "board.h"
#include <lcd_ssd1306.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#ifdef USE_MYVSPRINTF
#include <myprintf.h>
#endif

#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

#if LCD_ORIENTATION == LCD_ORIENTATION_PORTRAIT
#define SSD1306_GET_BUFFER_OFFSET(x, y) ((LCD_HEIGHT * ((LCD_WIDTH >> 3U) - 1)) + y - (x >> 3U) * LCD_HEIGHT)
#define SSD1306_GET_BUFFER_BIT(x, y) (7 - (x & 7U))
#elif LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE
#define SSD1306_GET_BUFFER_OFFSET(x, y) (x + (y >> 3U) * LCD_WIDTH)
#define SSD1306_GET_BUFFER_BIT(x, y) (y & 7U)
#elif LCD_ORIENTATION == LCD_ORIENTATION_PORTRAIT_REVERSED
#define SSD1306_GET_BUFFER_OFFSET(x, y) (LCD_HEIGHT - 1 - y + (x >> 3U) * LCD_HEIGHT)
#define SSD1306_GET_BUFFER_BIT(x, y) (x & 7U)
#elif LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE_REVERSED
#define SSD1306_GET_BUFFER_OFFSET(x, y) (((LCD_WIDTH * LCD_HEIGHT) >> 3U) - 1 - x - (y >> 3U) * LCD_WIDTH)
#define SSD1306_GET_BUFFER_BIT(x, y) (7 - (y & 7U))
#else
  #error "Wrong LCD_ORIENTATION"
#endif

#define BUFFER_SIZE LCD_WIDTH * LCD_HEIGHT / 8

unsigned char ssd1306_buffer[BUFFER_SIZE];

void ssd1306_command(unsigned char c)
{
  // I2C
	SSD1306_I2C_Write(1, // buffer length
                  	0, // control, Co = 0, D/C = 0
                   	&c);
}

unsigned int LcdInit(void)
{
#ifdef SH1106
  delayms(50);
#endif

// Init sequence
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80

  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(LCD_HWHEIGHT - 1);

  ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
  ssd1306_command(0x0);                                   // no offset
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
  ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
#ifdef SSD1306_EXTERNALVCC
    ssd1306_command(0x10);
#else
    ssd1306_command(0x14);
#endif
  ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
  ssd1306_command(0x00);                                  // 0x0 act like ks0108
  ssd1306_command(SSD1306_SEGREMAP | 0x1);
  ssd1306_command(SSD1306_COMSCANDEC);

 #if defined SSD1306_128_32
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x02);
	LCD_SetContrast(LCD_MAX_CONTRAST);

#elif defined SSD1306_128_64
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x12);
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
#ifdef SSD1306_EXTERNALVCC
	ssd1306_command(0x9F);
#else
  ssd1306_command(0xCF);
#endif
		
#elif defined SSD1306_96_16
  ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
  ssd1306_command(0x2);   //ada x12
  ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
#ifdef SSD1306_EXTERNALVCC
  ssd1306_command(0x10);
#else
  ssd1306_command(0xAF);
#endif
		
#endif

  ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
#ifdef SSD1306_EXTERNALVCC
  ssd1306_command(0x22);
#else
  ssd1306_command(0xF1);
#endif
  ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
  ssd1306_command(0x40);
  ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
  ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6

  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel

  return 1;
}

void internalDrawPixel(unsigned int lX, unsigned int lY, unsigned int Value)
{
	switch (Value)
	{
		case BLACK_COLOR: ssd1306_buffer[SSD1306_GET_BUFFER_OFFSET(lX, lY)] &=  ~(1 << SSD1306_GET_BUFFER_BIT(lX, lY)); break;
		default: ssd1306_buffer[SSD1306_GET_BUFFER_OFFSET(lX, lY)] |=  (1 << SSD1306_GET_BUFFER_BIT(lX, lY)); break;
	}
}

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
void LcdPixelDraw(unsigned int lX, unsigned int lY, unsigned int Value)
{
  internalDrawPixel(lX, lY, Value);
}

void LcdScreenFill(unsigned int color)
{
  memset(ssd1306_buffer, color == BLACK_COLOR ? 0 : 0xFF, BUFFER_SIZE);
}

void LCD_SetContrast(unsigned char contrast)
{
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

unsigned int LcdUpdate(void)
{
	int i;
	unsigned char *buffer = ssd1306_buffer;

#ifndef SH1106
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(LCD_HWWIDTH - 1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)
  #if LCD_HWHEIGHT == 64
    ssd1306_command(7); // Page end address
  #endif
  #if LCD_HWHEIGHT == 32
    ssd1306_command(3); // Page end address
  #endif
  #if LCD_HWHEIGHT == 16
    ssd1306_command(1); // Page end address
  #endif
#endif

// I2C

#ifdef SH1106
  for (i = 0; i< LCD_HWHEIGHT/8; i++)
  {
    ssd1306_command(0xB0 + i);
    ssd1306_command(2);
    ssd1306_command(0x10);
    SSD1306_I2C_Write(LCD_HWWIDTH, 0x40, buffer);
    buffer += LCD_HWWIDTH;
  }
#else
  for (i = 0; i< BUFFER_SIZE;)
	{
    // send a bunch of data in one xmission
  	SSD1306_I2C_Write(16, 0x40, buffer);
		buffer += 16;
		i+= 16;
  }
#endif

  return 1;
}

void LcdInvertDisplay(int invert)
{
  if (invert)
    ssd1306_command(SSD1306_INVERTDISPLAY);
  else
    ssd1306_command(SSD1306_NORMALDISPLAY);
}

void LcdPrintf(const char *format, unsigned int column, unsigned int row, const FONT_INFO *f, int white_on_black, ...)
{
  static char buffer[LCD_PRINTF_BUFFER_LENGTH];
  va_list vArgs;
  int textColor, bkColor;

  va_start(vArgs, white_on_black);
#ifdef USE_VSNPRINTF
  vsnprintf(buffer, sizeof(buffer), format, vArgs);
#else
#ifdef USE_MYVSPRINTF
  myvsprintf(buffer, format, vArgs);
#else
  vsprintf_s(buffer, sizeof(buffer), format, vArgs);
#endif
#endif
  va_end(vArgs);
  textColor = white_on_black ? WHITE_COLOR : BLACK_COLOR;
  bkColor = white_on_black ? BLACK_COLOR : WHITE_COLOR;
  LcdDrawText(column, row, buffer, f, textColor, bkColor, NULL);
}
