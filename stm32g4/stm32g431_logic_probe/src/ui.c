#include "board.h"
#include "ui.h"
#include <i2c.h>
#include <lcd_ssd1306.h>
#include <string.h>
#include <ws2812_spi.h>
#include <fonts/font10.h>

#define MAX_UH_VOLTAGES 5
#define MAX_UL_VOLTAGES 5

#define LINE2_Y 10
#define LINE3_Y 22
#define F_X 20
#define UH_X 31
#define UL_X 97

static const unsigned int uh_voltages[MAX_UH_VOLTAGES] = {
  2500,
  2000,
  1500,
  1000,
  880
};

static const unsigned int ul_voltages[MAX_UL_VOLTAGES] = {
  800,
  400,
  300,
  200,
  100
};

static ws2812_rgb led_data[WS2812_MAX_LEDS];
static int counter;
static int uh_index, ul_index;
static unsigned int uh_changed_to, ul_changed_to;
static int prev_button1_pressed, prev_button2_pressed;

void UI_Init(void)
{
  counter = 0;
  memset(led_data, 0, sizeof(led_data));
  uh_index = 0;
  ul_index = 1;
  uh_changed_to = DEFAULT_DAC3_VOLTAGE;
  ul_changed_to = DEFAULT_DAC1_VOLTAGE;
  prev_button1_pressed = prev_button2_pressed = 0;

  I2C_Write(I2C2, SSD1306_I2C_ADDRESS, NULL, 0, I2C_TIMEOUT); // workaround
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  LcdDrawText(0, 0, "FH", &courierNew10ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(0, LINE2_Y, "FL", &courierNew10ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(0, LINE3_Y, "UH", &courierNew10ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(64, LINE3_Y, "UL", &courierNew10ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
}

static void __attribute__((section(".RamFunc"))) calculate_led_data(void)
{
  led_data[0].red = counter_low * WS2812_MAX_VALUE / COUNTERS_MAX;
  unsigned int zero_one_sum = counter_low + counter_high;
  unsigned char floating_value;
  if (zero_one_sum >= COUNTERS_MAX)
    floating_value = 0;
  else
    floating_value = (COUNTERS_MAX - zero_one_sum) * WS2812_MAX_VALUE / (COUNTERS_MAX * 2);
  led_data[1].green = floating_value;
  led_data[1].red = floating_value;
  led_data[2].green = counter_high * WS2812_MAX_VALUE / COUNTERS_MAX;
  int pulse = counter_freq_high && counter_freq_low;
  led_data[3].blue = pulse ? WS2812_MAX_VALUE : 0;
}

void __attribute__((section(".RamFunc"))) ShowFrequency(int x, int y, unsigned int frequency)
{
  frequency *= 10;
  LcdPrintf("%2d.%03d.%03d", x, y, &courierNew10ptFontInfo, 1,
            frequency / 1000000, (frequency / 1000) % 1000, frequency % 1000);
}

void __attribute__((section(".RamFunc"))) ShowVoltage(int x, int y, unsigned int voltage)
{
  LcdPrintf("%d.%d", x, y, &courierNew10ptFontInfo, 1,
            voltage / 1000, (voltage / 100) % 10);
}

void __attribute__((section(".RamFunc"))) Process_Timer_Event(void)
{
  calculate_led_data();
  ws2812_send(0, (const ws2812_rgb *)&led_data, WS2812_MAX_LEDS);

  int pressed = BUTTON1_PRESSED;
  if (pressed && !prev_button1_pressed)
  {
    uh_index++;
    if (uh_index >= MAX_UH_VOLTAGES)
      uh_index = 0;
    uh_changed_to = uh_voltages[uh_index];
    set_h_voltage(uh_changed_to);
  }
  prev_button1_pressed = pressed;
  pressed = BUTTON2_PRESSED;
  if (pressed && !prev_button2_pressed)
  {
    ul_index++;
    if (ul_index >= MAX_UL_VOLTAGES)
      ul_index = 0;
    ul_changed_to = ul_voltages[ul_index];
    set_l_voltage(ul_changed_to);
  }
  prev_button2_pressed = pressed;

  counter++;
  if (counter == 5)
  {
    counter = 0;
    ShowFrequency(F_X, 0, counter_freq_high);
    ShowFrequency(F_X, LINE2_Y, counter_freq_low);
    if (uh_changed_to)
    {
      ShowVoltage(UH_X, LINE3_Y, uh_changed_to);
      uh_changed_to = 0;
    }
    if (ul_changed_to)
    {
      ShowVoltage(UL_X, LINE3_Y, ul_changed_to);
      ul_changed_to = 0;
    }
    LcdUpdate();
  }
}
