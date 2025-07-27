#include "board.h"
#include <lcd_ssd1331.h>
#include <spi_lcd_common.h>

#define SSD1331_CMD_SETCOLUMN      0x15     //!< Set column address
#define SSD1331_CMD_SETROW         0x75        //!< Set row adress
#define SSD1331_CMD_CONTRASTA      0x81     //!< Set contrast for color A
#define SSD1331_CMD_CONTRASTB      0x82     //!< Set contrast for color B
#define SSD1331_CMD_CONTRASTC      0x83     //!< Set contrast for color C
#define SSD1331_CMD_MASTERCURRENT  0x87 //!< Master current control
#define SSD1331_CMD_PRECHARGEA     0x8A //!< Set second pre-charge speed for color A
#define SSD1331_CMD_PRECHARGEB     0x8B //!< Set second pre-charge speed for color B
#define SSD1331_CMD_PRECHARGEC     0x8C //!< Set second pre-charge speed for color C
#define SSD1331_CMD_SETREMAP       0xA0      //!< Set re-map & data format
#define SSD1331_CMD_STARTLINE      0xA1     //!< Set display start line
#define SSD1331_CMD_DISPLAYOFFSET  0xA2 //!< Set display offset
#define SSD1331_CMD_NORMALDISPLAY  0xA4 //!< Set display to normal mode
#define SSD1331_CMD_DISPLAYALLON   0xA5  //!< Set entire display ON
#define SSD1331_CMD_DISPLAYALLOFF  0xA6 //!< Set entire display OFF
#define SSD1331_CMD_INVERTDISPLAY  0xA7 //!< Invert display
#define SSD1331_CMD_SETMULTIPLEX   0xA8  //!< Set multiplex ratio
#define SSD1331_CMD_DIMMODE        0xAB  //!< Configure dim mode
#define SSD1331_CMD_SETMASTER      0xAD     //!< Set master configuration
#define SSD1331_CMD_DISPLAYDIM     0xAC    //!< Display on in dim mode
#define SSD1331_CMD_DISPLAYOFF     0xAE    //!< Display OFF (sleep mode)
#define SSD1331_CMD_DISPLAYON      0xAF     //!< Normal Brightness Display ON
#define SSD1331_CMD_POWERSAVE      0xB0     //!< Power save mode
#define SSD1331_CMD_PRECHARGE      0xB1     //!< Phase 1 and 2 period adjustment
#define SSD1331_CMD_CLOCKDIV       0xB3 //!< Set display clock divide ratio/oscillator frequency
#define SSD1331_CMD_MASTERTABLE    0xB8     //!< Set master look up table for grayscale
#define SSD1331_CMD_BUILTINLUT     0xB9     //!< Use builtin linear LUT
#define SSD1331_CMD_PRECHARGELEVEL 0xBB //!< Set pre-charge voltage
#define SSD1331_CMD_VCOMH          0xBE          //!< Set Vcomh voltge
#define SSD1357_CMD_SETCOMMANDLOCK 0xFD

#define SSD1331_CMD_DRAWLINE       0x21      //!< Draw line
#define SSD1331_CMD_DRAWRECT       0x22      //!< Draw rectangle
#define SSD1331_CMD_CLEARWINDOW    0x25      //!< Clear window
#define SSD1331_CMD_FILL           0x26      //!< Fill enable/disable

static void SSD1331_Reset(void)
{
  LCD_RST_PIN_SET;
  delay(5);
  LCD_RST_PIN_CLR;
  delay(5);
  LCD_RST_PIN_SET;
  delayms(100);
}

static void LcdScreenClear(void)
{
  unsigned char data[5];
  data[0] = SSD1331_CMD_CLEARWINDOW;
  data[1] = 0;
  data[2] = 0;
  data[3] = LCD_WIDTH - 1;
  data[4] = LCD_HEIGHT - 1;
  Lcd_SendCommands(data, 5);
}

void LcdInit(unsigned char madctl)
{
  SSD1331_Reset();
  Lcd_SendCommand(SSD1331_CMD_SETMULTIPLEX);
  Lcd_SendCommand(0x3F);
  Lcd_SendCommand(SSD1331_CMD_SETREMAP);
  Lcd_SendCommand(madctl | 0x60);
  Lcd_SendCommand(SSD1331_CMD_DISPLAYON);
  LcdScreenFill(BLACK_COLOR);
}

/*void LcdRectFill(unsigned int column, unsigned int row, unsigned int dx, unsigned int dy, unsigned int color)
{
  unsigned char data[11];
  data[0] = SSD1331_CMD_DRAWRECT;
  data[1] = column;
  data[2] = row;
  data[3] = column + dx - 1;
  data[4] = row + dy - 1;
  unsigned char c1 = (unsigned char)color;
  unsigned char c2 = (unsigned char)(color >> 8);
  unsigned char c3 = (unsigned char)(color >> 16);
  data[5] = c1;
  data[6] = c2;
  data[7] = c3;
  data[8] = c1;
  data[9] = c2;
  data[10] = c3;
  Lcd_SendCommands(data, 11);
}*/

void LcdSetWindow(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  unsigned char data[3];

  data[0] = SSD1331_CMD_SETCOLUMN;
  data[1] = (unsigned char)x1;
  data[2] = (unsigned char)x2;
  Lcd_SendCommands(data, 3);

  data[0] = SSD1331_CMD_SETROW;
  data[1] = (unsigned char)y1;
  data[2] = (unsigned char)y2;
  Lcd_SendCommands(data, 3);
}