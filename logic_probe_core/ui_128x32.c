#include "board.h"
#include "ui.h"
#include <i2c.h>
#include <lcd_ssd1306.h>
#include <string.h>
#include <ws2812_spi.h>
#include <fonts/font10.h>
#include <fonts/font8_2.h>

#define LINE2_Y 10
#define LINE3_Y 23
#define F_X 20
#define SYMBOL_WIDTH 9
#define UH_X SYMBOL_WIDTH
#define UL_X (SYMBOL_WIDTH*9)
#define PH_X (SYMBOL_WIDTH*5)
#define PL_X (SYMBOL_WIDTH*11)

static int counter;

void UI_Init(void)
{
  counter = 0;

  UI_CommonInit();

  I2C_Write(I2C2, SSD1306_I2C_ADDRESS, NULL, 0, I2C_TIMEOUT); // workaround
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  LcdDrawText(0, 0, "FH", &courierNew10ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(0, LINE2_Y, "FL", &courierNew10ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(0, LINE3_Y, "U", &courierNew8ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(SYMBOL_WIDTH*4, LINE3_Y, "%", &courierNew8ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(SYMBOL_WIDTH*8, LINE3_Y, "H", &courierNew8ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(SYMBOL_WIDTH*10, LINE3_Y, "%", &courierNew8ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
}

static void RAMFUNC ShowFrequency(int x, int y, unsigned int frequency)
{
  frequency *= 10;
  LcdPrintf("%2d.%03d.%03d", x, y, &courierNew10ptFontInfo, 1,
            frequency / 1000000, (frequency / 1000) % 1000, frequency % 1000);
}

static void RAMFUNC ShowVoltmeter(int x, int y, unsigned int uv)
{
  LcdPrintf("%3d.%06d", x, y, &courierNew10ptFontInfo, 1,
            uv / 1000000, uv % 1000000);
}

static void RAMFUNC ShowVoltage(int x, int y, unsigned int voltage)
{
  LcdPrintf("%d.%d", x, y, &courierNew8ptFontInfo, 1,
            voltage / 1000, (voltage / 100) % 10);
}

static void RAMFUNC ShowVoltage1(int x, int y, unsigned int voltage)
{
  LcdPrintf("%d", x, y, &courierNew8ptFontInfo, 1, voltage / 100);
}

static void RAMFUNC ShowPercentage(int x, int y, unsigned int percentage)
{
  LcdPrintf("%3d", x, y, &courierNew8ptFontInfo, 1, percentage);
}

void RAMFUNC DrawMode(void)
{
  LcdDrawText(0, 0, voltmeter_mode ? "V " : "FH", &courierNew10ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
}

void RAMFUNC Process_Timer_Event(void)
{
  calculate_led_data_without_rs();
  ws2812_send(0, (const ws2812_rgb *)&led_data, WS2812_MAX_LEDS);

  Process_Button_Events();

  counter++;
  if (counter == 5)
  {
    counter = 0;
    if (voltmeter_mode)
    {
      ShowVoltmeter(F_X, 0, get_adc_voltage());
      adc_start();
    }
    else
      ShowFrequency(F_X, 0, counter_freq_high);
    ShowFrequency(F_X, LINE2_Y, counter_freq_low);
    ShowPercentage(PH_X, LINE3_Y, counter_high * 100 / COUNTERS_MAX);
    ShowPercentage(PL_X, LINE3_Y, counter_low * 100 / COUNTERS_MAX);
    if (uh_changed_to)
    {
      ShowVoltage(UH_X, LINE3_Y, uh_changed_to);
      uh_changed_to = 0;
    }
    if (ul_changed_to)
    {
      ShowVoltage1(UL_X, LINE3_Y, ul_changed_to);
      ul_changed_to = 0;
    }
    LcdUpdate();
  }
}
