#include <pico/time.h>
#include <hardware/gpio.h>
#include <pico/stdio.h>
#include "board.h"
#include <hardware/pio.h>
#include <stdio.h>

#define DATA_LENGTH 63000

static unsigned int data[DATA_LENGTH];

static void __no_inline_not_in_flash_func(collect_data)()
{
  unsigned int *data_p = data;
  int counter = DATA_LENGTH;

  LogicAnalyserStart();

  while (counter--)
    *data_p++ = pio_sm_get_blocking(CAPTURE_PIO, CAPTURE_SM);

  LogicAnalyserStop();
}

static void send_data()
{
  unsigned int *data_p = data, d;
  int counter = DATA_LENGTH;

  while (counter--)
  {
    d = *data_p++;
    putchar_raw((int)(d & 0xFF));
    putchar_raw((int)((d >> 8) & 0xFF));
    putchar_raw((int)((d >> 16) & 0xFF));
    putchar_raw((int)((d >> 24) & 0xFF));
  }
}

int main()
{
  bool led_state = false;
  int ch;

  SystemInit();

  while (1)
  {
    gpio_put(PICO_DEFAULT_LED_PIN, led_state);
    led_state = !led_state;
    ch = getchar_timeout_us(0);
    if (ch != 's')
    {
      sleep_ms(250);
      continue;
    }
    collect_data();
    send_data();
  }
}
