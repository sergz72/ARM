#include "board.h"
#include <lcd_ssd1357.h>
#include <spi_lcd_common.h>

#define SSD1357_CMD_SETCOLUMN      0x15     //!< Set column address
#define SSD1357_CMD_SETROW         0x75        //!< Set row adress
#define SSD1357_CMD_SETREMAP       0xA0      //!< Set re-map & data format
#define SSD1357_CMD_STARTLINE      0xA1     //!< Set display start line
#define SSD1357_CMD_DISPLAYALLOFF  0xA4 //!< Set entire display OFF
#define SSD1357_CMD_DISPLAYALLON   0xA5  //!< Set entire display ON
#define SSD1357_CMD_NORMALDISPLAY  0xA6 //!< Set display to normal mode
#define SSD1357_CMD_INVERTDISPLAY  0xA7 //!< Invert display
#define SSD1357_CMD_DISPLAYOFF     0xAE    //!< Display OFF (sleep mode)
#define SSD1357_CMD_DISPLAYON      0xAF     //!< Normal Brightness Display ON
#define SSD1357_CMD_PRECHARGE      0xB1     //!< Phase 1 and 2 period adjustment
#define SSD1357_CMD_CLOCKDIV       0xB3 //!< Set display clock divide ratio/oscillator frequency
#define SSD1357_CMD_PRECHARGE2     0xB6     //!< Set second precharge period
#define SSD1357_CMD_MASTERTABLE    0xB8     //!< Set master look up table for grayscale
#define SSD1357_CMD_BUILTINLUT     0xB9     //!< Use builtin linear LUT
#define SSD1357_CMD_PRECHARGELEVEL 0xBB //!< Set pre-charge voltage
#define SSD1357_CMD_LUTTABLEA      0xBC     //!< Set invividual lookup table for grayscale (color A)
#define SSD1357_CMD_LUTTABLEC      0xBD     //!< Set invividual lookup table for grayscale (color C)
#define SSD1357_CMD_VCOMH          0xBE          //!< Set Vcomh voltge
#define SSD1357_CMD_CONTRASTABC    0xC1     //!< Set contrast for colors A,B,C
#define SSD1357_CMD_MASTERCURRENT  0xC7     //!< Set master contrast current control
#define SSD1357_CMD_SETMULTIPLEX   0xCA  //!< Set multiplex ratio
#define SSD1357_CMD_SETCOMMANDLOCK 0xFD

#define SSD1331_CMD_DRAWLINE       0x21      //!< Draw line
#define SSD1331_CMD_DRAWRECT       0x22      //!< Draw rectangle
#define SSD1331_CMD_FILL           0x26          //!< Fill enable/disable
#define SSD1331_CMD_NORMALDISPLAY  0xA4 //!< Set display to normal mode
#define SSD1331_CMD_DISPLAYALLON   0xA5  //!< Set entire display ON
#define SSD1331_CMD_DISPLAYALLOFF  0xA6 //!< Set entire display OFF
#define SSD1331_CMD_INVERTDISPLAY  0xA7 //!< Invert display
#define SSD1331_CMD_CONTRASTA      0x81     //!< Set contrast for color A
#define SSD1331_CMD_CONTRASTB      0x82     //!< Set contrast for color B
#define SSD1331_CMD_CONTRASTC      0x83     //!< Set contrast for color C
#define SSD1331_CMD_MASTERCURRENT  0x87 //!< Master current control
#define SSD1331_CMD_SETMULTIPLEX   0xA8  //!< Set multiplex ratio
#define SSD1331_CMD_SETMASTER      0xAD     //!< Set master configuration
#define SSD1331_CMD_POWERMODE      0xB0     //!< Power save mode
#define SSD1331_CMD_PRECHARGEA     0x8A //!< Set second pre-charge speed for color A
#define SSD1331_CMD_PRECHARGEB     0x8B //!< Set second pre-charge speed for color B
#define SSD1331_CMD_PRECHARGEC     0x8C //!< Set second pre-charge speed for color C

static void SSD1357_Reset(void)
{
  LCD_RST_PIN_SET;
  delay(5);
  LCD_RST_PIN_CLR;
  delay(5);
  LCD_RST_PIN_SET;
  delayms(300);
}

void LcdInit(unsigned char madctl)
{
  unsigned char data[2];
  SSD1357_Reset();
  Lcd_SendCommand(SSD1357_CMD_SETMULTIPLEX);
  data[0] = 0x3F;
  Lcd_SendData(data, 1);
  Lcd_SendCommand(SSD1357_CMD_DISPLAYOFFSET);
  data[0] = madctl & SSD1357_MADCTL_Y_MIRROR ? 0x40 : 0;
  Lcd_SendData(data, 1);
  Lcd_SendCommand(SSD1357_CMD_SETREMAP);
  data[0] = madctl | 0x60;
  data[1] = 0;
  Lcd_SendData(data, 2);
  Lcd_SendCommand(SSD1357_CMD_DISPLAYON);
  LcdScreenFill(BLACK_COLOR);
}

void LcdSetWindow(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  unsigned char data[2];

  x1 += 0x20;
  x2 += 0x20;

  data[0] = (unsigned char)x1;
  data[1] = (unsigned char)x2;
  Lcd_SendCommand(SSD1357_CMD_SETCOLUMN);
  Lcd_SendData(data, 2);

  data[0] = (unsigned char)y1;
  data[1] = (unsigned char)y2;

  Lcd_SendCommand(SSD1357_CMD_SETROW);
  Lcd_SendData(data, 2);

  Lcd_SendCommand(SSD1357_CMD_WRITERAM);
}