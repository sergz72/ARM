#include "board.h"
#include <lcd_st7789.h>
#include <spi_lcd_common.h>

#define ST7789_NOP       0x00
#define ST7789_SWRESET   0x01 // software reset
#define ST7789_RDDID     0x04 // read display id
#define ST7789_RDDST     0x09 // read display status
#define ST7789_RDDPM     0x0A // read display power mode
#define ST7789_RDDMADCTL 0x0B // read display MADCTL
#define ST7789_RDDCOLMOD 0x0C // read display pixel format
#define ST7789_RDDIM     0x0D // read display image mode
#define ST7789_RDDSM     0x0E // read display signal mode
#define ST7789_RDDSDR    0x0F // read display self-diagnostic result

#define ST7789_SLPIN   0x10 // sleep in
#define ST7789_SLPOUT  0x11 // sleep out
#define ST7789_PTLON   0x12 // partial display mode on
#define ST7789_NORON   0x13 // normal display mode on

#define ST7789_INVOFF  0x20 // display inversion off
#define ST7789_INVON   0x21 // display inversion on
#define ST7789_GAMSET  0x26 // gamma set
#define ST7789_DISPOFF 0x28 // display off
#define ST7789_DISPON  0x29 // display on
#define ST7789_CASET   0x2A // column address set
#define ST7789_RASET   0x2B // row address set
#define ST7789_RAMWR   0x2C // memory write
#define ST7789_RAMRD   0x2E // memory read

#define ST7789_PTLAR   0x30 // partial area
#define ST7789_VSCRDEF 0x33 // vertical scrolling definition
#define ST7789_TEOFF   0x34 // tearing effect line off
#define ST7789_TEON    0x35 // tearing effect line on
#define ST7789_MADCTL  0x36 // memory data access control
#define ST7789_VSCSAD  0x37 // vertical scroll start address in RAM
#define ST7789_IDMOFF  0x38 // idle mode off
#define ST7789_IDMON   0x39 // idle mode on
#define ST7789_COLMOD  0x3A // interface pixel format
#define ST7789_WRMEMC  0x3C // write memory continue
#define ST7789_RDMEMC  0x3E // read memory continue

#define ST7789_STE     0x44 // set tear scanline
#define ST7789_GSCAN   0x45 // get scanline

#define ST7789_WRDISBV  0x51 // write display brightness
#define ST7789_RDDISBV  0x52 // read display brightness
#define ST7789_WRCTRLD  0x53 // write CTRL display
#define ST7789_RDCTRLD  0x54 // read CTRL display
#define ST7789_WRCACE   0x55 // write content adaptive brightness control and color enhancement
#define ST7789_RDCABC   0x56 // read content adaptive brightness control
#define ST7789_WRCABCMB 0x5E // write CABC minimum brightness
#define ST7789_RDCABCMB 0x5F // read CABC minimum brightness

#define ST7789_RDABCSDR 0x68 // Read Automatic Brightness Control Self-Diagnostic Result

#define ST7789_RAMCTRL 0xB0 // ram control
#define ST7789_RGBCTRL 0xB1 // RGB interface control
#define ST7789_PORCTRL 0xB2 // porch setting
#define ST7789_FRCTLT1 0xB3 // frame rate control 1
#define ST7789_PARCTRL 0xB5 // partial control
#define ST7789_GCTRL   0xB7 // gate control
#define ST7789_GTADJ   0xB8 // gate on timing adjustment
#define ST7789_DGMEN   0xBA // digital gamma enable
#define ST7789_VCOMS   0xBB // VCOM setting
#define ST7789_POWSAVE 0xBC // power saving mode
#define ST7789_DLPOFFSAVE 0xBD // display off power save

#define ST7789_LCMCTRL  0xC0 // lcm control
#define ST7789_IDSET    0xC1 // id code setting
#define ST7789_VDVVRHEN 0xC2 // VDV and VRH command enable
#define ST7789_VRHS     0xC3 // VRH set
#define ST7789_VDVS     0xC4 // VDV set
#define ST7789_VCMOFSET 0xC5 // VCM offset set
#define ST7789_FRCTLT2  0xC6 // frame rate control in normal mode
#define ST7789_CABCCTRL 0xC7 // CABC control
#define ST7789_REGSEL1  0xC8 // register value selection 1
#define ST7789_REGSEL2  0xCA // register value selection 2
#define ST7789_PWMFRSEL 0xCC // PWM frequency selection

#define ST7789_PWCTRL1  0xD0 // power control 1
#define ST7789_VAPVANEN 0xD2 // enable VAP/VAN signal output
#define ST7789_CMD2EN   0xDF // command 2 enable


#define ST7789_RDID1   0xDA // read id1
#define ST7789_RDID2   0xDB // read id2
#define ST7789_RDID3   0xDC // read id3
#define ST7789_RDID4   0xDD // read id4

#define ST7789_PVGAMCTRL 0xE0 // Positive Voltage Gamma Control
#define ST7789_NVGAMCTRL 0xE1 // Negative Voltage Gamma Control
#define ST7789_DGMLUTR   0xE2 // Digital Gamma Look-up Table for Red
#define ST7789_DGMLUTB   0xE3 // Digital Gamma Look-up Table for Blue
#define ST7789_GATECTRL  0xE4 // Gate Control
#define ST7789_SPI2EN    0xE7 // SPI2 enable
#define ST7789_PWCTRL2   0xE8 // power control 2
#define ST7789_EQCTRL    0xE9 // Equalize time control

#define ST7789_PROMEN  0xFA // Program Mode Enable
#define ST7789_NVMSET  0xFC // NVM setting
#define ST7789_PROMACT 0xFE // Program Action

#define ST7789_COLMOD_16_BPP 0x55  // 01010101 - 65K of RGB interface, 16-bit/pixel

#define ST7789_XOFFSET 40
#define ST7789_YOFFSET 53

#ifndef NO_ST7789_RESET
static void ST7789_Reset(void)
{
  LCD_RST_PIN_SET;
  delay(20);
  LCD_RST_PIN_CLR;
  delay(20); // > 10 us
  LCD_RST_PIN_SET;
  delayms(120);
}
#endif

static void ST7789_InitReg(unsigned char madctl)
{
  unsigned char data;

  //Sleep exit
  Lcd_SendCommand(ST7789_SLPOUT);
  Lcd_CS_High();
  delayms(5);
  // Set Interface Pixel Format - 16-bit/pixel
  Lcd_SendCommand(ST7789_INVON);

  Lcd_SendCommand(ST7789_COLMOD);
  data = ST7789_COLMOD_16_BPP;
  Lcd_SendData(&data, 1);

  Lcd_SendCommand(ST7789_MADCTL);
  data = madctl;
  Lcd_SendData(&data, 1);

  Lcd_SendCommand(ST7789_DISPON);
  Lcd_CS_High();
}

void LcdInit(unsigned char madctl)
{
#ifndef NO_ST7789_RESET
  ST7789_Reset();
#endif
  ST7789_InitReg(madctl);
  LcdScreenFill(BLACK_COLOR);
}

void LcdSetWindow(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  unsigned char data[4];

  x1 += ST7789_XOFFSET;
  x2 += ST7789_XOFFSET;
  y1 += ST7789_YOFFSET;
  y2 += ST7789_YOFFSET;

  data[0] = (unsigned char)(x1 >> 8);
  data[1] = (unsigned char)x1;
  data[2] = (unsigned char)(x2 >> 8);
  data[3] = (unsigned char)x2;
  Lcd_SendCommand(ST7789_CASET);
  Lcd_SendData(data, 4);

  data[0] = (unsigned char)(y1 >> 8);
  data[1] = (unsigned char)y1;
  data[2] = (unsigned char)(y2 >> 8);
  data[3] = (unsigned char)y2;

  Lcd_SendCommand(ST7789_RASET);
  Lcd_SendData(data, 4);

  Lcd_SendCommand(ST7789_RAMWR);
}
