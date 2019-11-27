#include "board.h"
#include <lcd_ssd1306.h>
#include <string.h>

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

unsigned char ssd1306_buffer[LCD_HEIGHT * LCD_WIDTH / 8];

void ssd1306_command(unsigned char c)
{
  // I2C
	SSD1306_I2C_Write(1, // buffer length
                  	0, // control, Co = 0, D/C = 0
                   	&c);
//  Wire.beginTransmission(_i2caddr);
//  Wire.write(0x00); // Co = 0, D/C = 0
//  Wire.write(c);
//  Wire.endTransmission();
}

void LcdInit(const FONT_INFO *sysfont)
{
// Init sequence
  ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
  ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
  ssd1306_command(0x80);                                  // the suggested ratio 0x80

  ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
  ssd1306_command(LCD_HEIGHT - 1);

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

  cursorDC = NULL;

  LcdInitDC(&screenDC, 0, 0, LCD_WIDTH, LCD_HEIGHT, WHITE_COLOR, BLACK_COLOR, sysfont);

  LcdClearDC(NULL);
}

void InternalPixelDraw(unsigned int lX, unsigned int lY, unsigned int Value)
{
	switch (Value)
	{
		case BLACK_COLOR: ssd1306_buffer[lX + (lY / 8) * LCD_WIDTH] &=  ~(1 << (lY & 7)); break;
		default: ssd1306_buffer[lX + (lY / 8) * LCD_WIDTH] |=  (1 << (lY & 7)); break;
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
void LcdPixelDraw(DC *dc, unsigned int lX, unsigned int lY, unsigned int Value)
{
  if (!dc)
    dc = &screenDC;

  lX += dc->left;
  if (lX > dc->right)
    return;

  lY += dc->top;
  if (lY > dc->bottom)
    return;

	InternalPixelDraw(lX, lY, Value);
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
void LcdLineDrawH(DC *dc, unsigned int lX1, unsigned int lX2, unsigned int lY, unsigned int Value)
{
  if (lX2 < lX1)
    return;

  if (!dc)
    dc = &screenDC;

  lX1 += dc->left;
  if (lX1 > dc->right)
    return;

  lY += dc->top;
  if (lY > dc->bottom)
    return;

  lX2 += dc->left;
  if (lX2 > dc->right)
    lX2 = dc->right;
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
void LcdLineDrawV(DC *dc, unsigned int lX, unsigned int lY1, unsigned int lY2, unsigned int Value)
{
  if (lY2 < lY1)
    return;

  if (!dc)
    dc = &screenDC;

  lX += dc->left;
  if (lX > dc->right)
    return;

  lY1 += dc->top;
  if (lY1 > dc->bottom)
    return;

  lY2 += dc->top;
  if (lY2 > dc->bottom)
    lY2 = dc->bottom;
}

void LcdClearDC(DC *dc)
{
  if (!dc)
    dc = &screenDC;

	if (dc == &screenDC)
		memset(ssd1306_buffer, 0, (LCD_WIDTH * LCD_HEIGHT / 8));
  else	
    LcdRectFill(dc, 0, 0, LcdGetWidth(dc), LcdGetHeight(dc), dc->bkColor);

  dc->cursorX = dc->cursorY = dc->cursor_state;
}

void LcdRectFill(DC *dc, unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color)
{
  unsigned int xMax, yMax;
  int lX, lY;

  if (!dc)
    dc = &screenDC;

  column += dc->left;
  if (column > dc->right)
    return;

  row += dc->top;
  if (row > dc->bottom)
    return;

  xMax = column + dx - 1;
  if (xMax > dc->right)
    xMax = dc->right;

  yMax = row + dy - 1;
  if (yMax > dc->bottom)
    yMax = dc->bottom;

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
			InternalPixelDraw(lX, lY, color);
		}
  }
}

unsigned int LcdDrawText(DC *dc, unsigned int column, unsigned int row, char *text)
{
  unsigned int line, width, xCount, yCount, i;
  unsigned int *buf;

  if (!dc)
    dc = &screenDC;

  column += dc->left;
  if (column > dc->right)
    return 0;

  row += dc->top;
  if (row > dc->bottom)
    return 0;

  width = xCount = 0;

  yCount = dc->font->char_height;
  if (row + yCount - 1 > dc->bottom)
    yCount = dc->bottom - row + 1;

  for (line = 0; line < yCount; line++)
  {
    width = lcd_prepare_data_buffer(line, text, dc->font, dc->textColor, dc->bkColor);
    if (!xCount)
    {
      xCount = width;
      if (column + xCount - 1 > dc->right)
        xCount = dc->right - column + 1;
    }

    buf = display_data_buffer;

    for(i = 0; i < xCount; i++)
			InternalPixelDraw(column + i, row, *buf++);

    row++;
  }

  return width;
}

void LcdScroll(DC *dc, short count)
{
}

void LCD_SetContrast(unsigned char contrast)
{
  ssd1306_command(SSD1306_SETCONTRAST);
  ssd1306_command(contrast);
}

void LCD_Update(void)
{
	int i;
	unsigned char *buffer = ssd1306_buffer;
	
  ssd1306_command(SSD1306_COLUMNADDR);
  ssd1306_command(0);   // Column start address (0 = reset)
  ssd1306_command(LCD_WIDTH - 1); // Column end address (127 = reset)

  ssd1306_command(SSD1306_PAGEADDR);
  ssd1306_command(0); // Page start address (0 = reset)
  #if LCD_HEIGHT == 64
    ssd1306_command(7); // Page end address
  #endif
  #if LCD_HEIGHT == 32
    ssd1306_command(3); // Page end address
  #endif
  #if LCD_HEIGHT == 16
    ssd1306_command(1); // Page end address
  #endif

// I2C
	
  for (i = 0; i< LCD_WIDTH*LCD_HEIGHT/8;)
	{
    // send a bunch of data in one xmission
  	SSD1306_I2C_Write(16, 0x40, buffer);
		buffer += 16;
		i+= 16;
  }
}

void LcdInvertDisplay(int invert)
{
  if (invert)
    ssd1306_command(SSD1306_INVERTDISPLAY);
  else
    ssd1306_command(SSD1306_NORMALDISPLAY);
}
