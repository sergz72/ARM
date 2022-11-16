#include "board.h"
#include <lcd_st7735.h>

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#if LCD_ORIENTATION == LCD_ORIENTATION_PORTRAIT
#define ST7735_GET_BUFFER_OFFSET(x, y) (LCD_HEIGHT * (LCD_WIDTH - 1 - x) + y)
#elif LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE
#define ST7735_GET_BUFFER_OFFSET(x, y) (LCD_WIDTH * y + x)
#elif LCD_ORIENTATION == LCD_ORIENTATION_PORTRAIT_REVERSED
#define ST7735_GET_BUFFER_OFFSET(x, y) (LCD_HEIGHT * y + x)
#elif LCD_ORIENTATION == LCD_ORIENTATION_LANDSCAPE_REVERSED
#define ST7735_GET_BUFFER_OFFSET(x, y) (LCD_HEIGHT * y + x)
#else
#error "Wrong LCD_ORIENTATION"
#endif

#define BUFFER_SIZE LCD_WIDTH * LCD_HEIGHT

static unsigned short st7735_buffer[BUFFER_SIZE];

static void ST7735_SendCommand(unsigned char Reg)
{
  ST7735_DC_PIN_CLR;
  ST7735_CS_PIN_CLR;
  ST7735_WriteBytes(&Reg, 1);
}

static void ST7735_SendData(unsigned char *data, unsigned int len)
{
  ST7735_DC_PIN_SET;
  ST7735_WriteBytes(data, len);
  ST7735_CS_PIN_SET;
}

static void ST7735_Reset(void)
{
  ST7735_RST_PIN_SET;
  delayms(200);
  ST7735_RST_PIN_CLR;
  delayms(200);
  ST7735_RST_PIN_SET;
  delayms(200);
}

static void ST7735_InitReg(void)
{
  unsigned char data[16];

  ST7735_SendCommand(ST7735_SLPOUT);//Sleep exit
  delayms(120);
  ST7735_SendCommand(ST7735_INVON);
  ST7735_SendCommand(ST7735_INVON);

  data[0] = 0x05;
  data[1] = 0x3A;
  data[2] = 0x3A;
  data[4] = 0x05;
  data[5] = 0x3A;
  data[6] = 0x3A;

  ST7735_SendCommand(ST7735_FRMCTR1);
  ST7735_SendData(data, 3);

  ST7735_SendCommand(ST7735_FRMCTR2);
  ST7735_SendData(data, 3);

  ST7735_SendCommand(ST7735_FRMCTR3);
  ST7735_SendData(data, 6);

  ST7735_SendCommand(ST7735_INVCTR);
  data[0] = 0x03;
  ST7735_SendData(data, 1);

  ST7735_SendCommand(ST7735_PWCTR1);
  data[0] = 0x62;
  data[0] = 0x02;
  data[0] = 0x04;
  ST7735_SendData(data, 3);

  ST7735_SendCommand(ST7735_PWCTR2);
  data[0] = 0xC0;
  ST7735_SendData(data, 1);

  ST7735_SendCommand(ST7735_PWCTR3);
  data[0] = 0x0D;
  data[1] = 0x00;
  ST7735_SendData(data, 2);

  ST7735_SendCommand(ST7735_PWCTR4);
  data[0] = 0x8D;
  data[1] = 0x6A;
  ST7735_SendData(data, 2);

  ST7735_SendCommand(ST7735_PWCTR5);
  data[1] = 0xEE;
  ST7735_SendData(data, 2);

  ST7735_SendCommand(ST7735_VMCTR1);  /*VCOM*/
  data[0] = 0x0E;
  ST7735_SendData(data, 1);

  ST7735_SendCommand(ST7735_GMCTRP1);
  data[0] = 0x10;
  data[1] = 0x0E;
  data[2] = 0x02;
  data[3] = 0x03;
  data[4] = 0x0E;
  data[5] = 0x07;
  data[6] = 0x02;
  data[7] = 0x07;
  data[8] = 0x0A;
  data[9] = 0x12;
  data[10] = 0x27;
  data[11] = 0x37;
  data[12] = 0x00;
  data[13] = 0x0D;
  data[14] = 0x0E;
  data[15] = 0x10;
  ST7735_SendData(data, 16);

  ST7735_SendCommand(ST7735_GMCTRN1);
  data[2] = 0x03;
  data[4] = 0x0F;
  data[5] = 0x06;
  data[7] = 0x08;
  data[9] = 0x13;
  data[10] = 0x26;
  data[11] = 0x36;
  ST7735_SendData(data, 16);

  ST7735_SendCommand(ST7735_COLMOD);
  data[0] = 0x05;
  ST7735_SendData(data, 1);

  ST7735_SendCommand(ST7735_MADCTL);
  data[0] = 0xA8;
  ST7735_SendData(data, 1);

  ST7735_SendCommand(ST7735_DISPON);
}

unsigned int LcdInit(void)
{
  ST7735_SET_PWM(90);
  ST7735_Reset();
  ST7735_InitReg();
  LcdScreenFill(BLACK_COLOR);
  return 1;
}

void internalDrawPixel(unsigned int lX, unsigned int lY, unsigned int Value)
{
  st7735_buffer[ST7735_GET_BUFFER_OFFSET(lX, lY)] = Value;
}

void LcdPixelDraw(unsigned int lX, unsigned int lY, unsigned int Value)
{
  internalDrawPixel(lX, lY, Value);
}

void LcdScreenFill(unsigned int color)
{
  int i;
  unsigned short *p = st7735_buffer;

  for (i = 0; i < BUFFER_SIZE; i++)
    *p++ = (unsigned short)color;
}

static void SetWindow(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
  unsigned char data[4];

  x1 += ST7735_XSTART;
  x2 += ST7735_XSTART;
  y1 += ST7735_YSTART;
  y2 += ST7735_YSTART;

  data[0] = (unsigned char)(x1 >> 8);
  data[1] = (unsigned char)x1;
  data[2] = (unsigned char)(x2 >> 8);
  data[3] = (unsigned char)x2;
  ST7735_SendCommand(ST7735_CASET);
  ST7735_SendData(data, 4);

  data[0] = (unsigned char)(y1 >> 8);
  data[1] = (unsigned char)y1;
  data[2] = (unsigned char)(y2 >> 8);
  data[3] = (unsigned char)y2;

  ST7735_SendCommand(ST7735_RASET);
  ST7735_SendData(data, 4);

  ST7735_SendCommand(ST7735_RAMWR);
}

unsigned int LcdUpdate(void)
{
  SetWindow(0, 0, LCD_HWWIDTH - 1, LCD_HWHEIGHT - 1);
  ST7735_SendData((unsigned char*)st7735_buffer, BUFFER_SIZE * sizeof(short));
  return 1;
}
