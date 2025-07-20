#include "board.h"
#include "ui.h"
#include <lcd.h>
#include <string.h>

#define FONT_WIDTH   8
#define CHAR_PUNKT   10
#define CHAR_F       11
#define CHAR_H       12
#define CHAR_L       13
#define CHAR_U       14
#define CHAR_PERCENT 15
#define CHAR_V       16
#define CHAR_SPACE   17
#define CHAR_Z       18
#define CHAR_z       19
#define MAX_CHARS    20

#define FREQUENCY_COLUMNS 8

const unsigned int frequency_columns[FREQUENCY_COLUMNS] = { 2, 3, 4, 6, 7, 8, 10, 11 };

const unsigned char font8_data[MAX_CHARS][FONT_WIDTH] =
{
  //0
  {0x0,0x3E,0x41,0x41,0x41,0x3E,0x0,0x0},
  //1
  {0x0,0x40,0x41,0x7F,0x40,0x40,0x0,0x0},
  //2
  {0x0,0x62,0x51,0x49,0x45,0x42,0x0,0x0},
  //3
  {0x0,0x22,0x41,0x49,0x49,0x36,0x0,0x0},
  //4
  {0x0,0x18,0x14,0x52,0x7F,0x50,0x0,0x0},
  //5
  {0x0,0x20,0x4F,0x49,0x49,0x31,0x0,0x0},
  //6
  {0x0,0x3C,0x4A,0x49,0x49,0x31,0x0,0x0},
  //7
  {0x0,0x3,0x1,0x61,0x19,0x7,0x0,0x0},
  //8
  {0x0,0x36,0x49,0x49,0x49,0x36,0x0,0x0},
  //9
  {0x0,0x46,0x49,0x49,0x29,0x1E,0x0,0x0},
  //.
  {0x0,0x0,0x40,0x40,0x0,0x0,0x0,0x0},
  //F
  {0x0,0x41,0x7F,0x49,0x9,0x3,0x0,0x0},
  //H
  {0x41,0x7F,0x49,0x8,0x49,0x7F,0x41,0x0},
  //L
  {0x0,0x41,0x7F,0x41,0x40,0x70,0x0,0x0},
  //U
  {0x1,0x3F,0x41,0x40,0x41,0x3F,0x1,0x0},
  //%
  {0x0,0xA,0x2D,0x5A,0x28,0x0,0x0,0x0},
  //V
  {0x1,0x1F,0x21,0x40,0x21,0x1F,0x1,0x0},
  // space
  {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
  //Z
  {0x0,0x63,0x51,0x49,0x45,0x63,0x0,0x0},
  //z
  {0x0,0x4C,0x64,0x54,0x4C,0x64,0x0,0x0}
};

static int counter;

extern unsigned char ssd1306_buffer[];

static void LcdDrawChar(int c, int x, int y)
{
  unsigned char *bp = ssd1306_buffer + (y << 7) + (x << 3);
  const unsigned char *fp = font8_data[c];
  memcpy(bp, fp, 8);
}

static void InitFrequencyLine(int y, int c2, int c13, int c14)
{
  LcdDrawChar(CHAR_F, 0, y);
  if (c2)
    LcdDrawChar(c2, 1, y);
  LcdDrawChar(CHAR_PUNKT, 6, y);
  LcdDrawChar(CHAR_PUNKT, 10, y);
  LcdDrawChar(0, 12, y);
  LcdDrawChar(c13, 13, y);
  LcdDrawChar(c14, 14, y);
}

void UI_Init(void)
{
  counter = 0;

  delayms(50);
  
  UI_CommonInit();

  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  InitFrequencyLine(0, CHAR_H, CHAR_U, CHAR_SPACE);
  InitFrequencyLine(1, 0, CHAR_H, CHAR_z);
  InitFrequencyLine(2, CHAR_L, CHAR_U, CHAR_SPACE);

  LcdDrawChar(CHAR_H, 0, 3);
  LcdDrawChar(CHAR_PERCENT, 4, 3);
  LcdDrawChar(CHAR_Z, 5, 3);
  LcdDrawChar(CHAR_PERCENT, 9, 3);
  LcdDrawChar(CHAR_L, 10, 3);
  LcdDrawChar(CHAR_PERCENT, 14, 3);
}

unsigned int LcdDrawText(unsigned int x, unsigned int y, const char *text,
                         const FONT_INFO *f, unsigned int textColor,
                         unsigned int bkColor, unsigned int *owidth)
{
  return 0;
}

void RAMFUNC DrawMode(void)
{
  if (voltmeter_mode)
  {
    LcdDrawChar(CHAR_V, 0, 0);
    LcdDrawChar(CHAR_SPACE, 1, 0);
  }
  else
  {
    LcdDrawChar(CHAR_F, 0, 0);
    LcdDrawChar(CHAR_H, 1, 0);
  }
}

static void RAMFUNC ShowFrequency(int y, unsigned long frequency)
{
  for (int i = FREQUENCY_COLUMNS - 1; i >= 0; i--)
  {
    int c;
    if (i < 2 && !frequency)
      c = CHAR_SPACE;
    else
    {
      c = frequency % 10;
      frequency /= 10;
    }
    LcdDrawChar(c, frequency_columns[i], y);
  }
}

static void RAMFUNC ShowVoltage(int y, unsigned int voltage)
{
  int c = voltage / 1000;
  LcdDrawChar(c, 14, y);
  c = (voltage / 100) % 10;
  LcdDrawChar(c, 15, y);
}

static void RAMFUNC ShowPercentage(int x, unsigned int percentage)
{
  int c;
  if (percentage >= 100)
  {
    c = percentage / 100;
    LcdDrawChar(c, x, 3);
  }
  else
    LcdDrawChar(CHAR_SPACE, x, 3);
  x++;
  if (percentage >= 10)
  {
    c = (percentage / 10) % 10;
    LcdDrawChar(c, x, 3);
  }
  else
    LcdDrawChar(CHAR_SPACE, x, 3);
  c = percentage % 10;
  x++;
  LcdDrawChar(c, x, 3);
}

void RAMFUNC Process_Timer_Event(void)
{
  calculate_led_data_with_rs();
  ws2812_send(0, (const ws2812_rgb *)&led_data, WS2812_MAX_LEDS);

  Process_Button_Events();
  
  /*if (DAC1CONL & 0x80)
    LED_GREEN_ON;
  else
    LED_GREEN_OFF;

  if (DAC2CONL & 0x80)
    LED_RED_ON;
  else
    LED_RED_OFF;
  
  if (CLC2CONLbits.LCOUT)
    LED_BLUE_ON;
  else
    LED_BLUE_OFF;*/

  counter++;
  if (counter == 5)
  {
    counter = 0;
    if (voltmeter_mode)
    {
      ShowFrequency(0, get_adc_voltage() / 10);
      adc_start();
    }
    else
      ShowFrequency(0, counter_freq_high);
    ShowFrequency(1, counter_freq_rs);
    ShowFrequency(2, counter_freq_low);
    ShowPercentage(1, counter_high * 100 / COUNTERS_MAX);
    ShowPercentage(6, counter_z * 100 / COUNTERS_MAX);
    ShowPercentage(11, counter_low * 100 / COUNTERS_MAX);
    if (uh_changed_to)
    {
      ShowVoltage(0, uh_changed_to);
      uh_changed_to = 0;
    }
    if (ul_changed_to)
    {
      ShowVoltage(2, ul_changed_to);
      ul_changed_to = 0;
    }
    LcdUpdate();
  }
}
