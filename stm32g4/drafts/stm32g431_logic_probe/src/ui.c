#include "board.h"
#include "ui.h"
#include <i2c.h>
#include <lcd_ssd1306.h>
#include <string.h>
#include <ws2812_spi.h>

static ws2812_rgb led_data[WS2812_MAX_LEDS];

void UI_Init(void)
{
  memset(led_data, 0, sizeof(led_data));
  I2C_Write(I2C2, SSD1306_I2C_ADDRESS, NULL, 0, I2C_TIMEOUT); // workaround
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  LcdUpdate();
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

void __attribute__((section(".RamFunc"))) Process_Timer_Event(void)
{
  calculate_led_data();
  ws2812_send(0, (const ws2812_rgb *)&led_data, WS2812_MAX_LEDS);

  //LcdUpdate();
}
