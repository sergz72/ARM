#include "board.h"
#include <lcd_sh1107.h>
#include <string.h>

#define SIZE (LCD_HEIGHT * LCD_WIDTH / 8)

extern int long_delay;
extern unsigned int long_press;
extern unsigned int key;

static unsigned char lcd_buffer[SIZE], *lcd_buffer_p = lcd_buffer;

volatile int capacitance_measurement_done;
static int capacitance_measurement_channel;

void discharge_off(void)
{
}

void discharge_on(void)
{
}

void charge_off(void)
{
}

void charge_on(int channel)
{
}

unsigned int get_capacitance_measurement_start_time(void)
{
  return 0;
}

unsigned int get_capacitance_measurement_end_time(void)
{
  return 0;//capacitance_measurement_channel == CAPACITANCE_CHANNEL_100K ? capacitance_value : capacitance_value / 100;
}

void capacitance_measurement_start(int channel)
{
  capacitance_measurement_channel = channel;
  capacitance_measurement_done = 1;
}

void SH1107_Write(int num_bytes, unsigned char dc, const unsigned char *buffer)
{
  if (dc)
  {
    memcpy(lcd_buffer_p, buffer, num_bytes);
    lcd_buffer_p += num_bytes;
    if (lcd_buffer_p - lcd_buffer >= SIZE)
      lcd_buffer_p = lcd_buffer;
  }
}

void delayms(unsigned int ms)
{

}

void delay(unsigned int us)
{

}

int get_lcd_buffer_bit(int x, int y)
{
  unsigned char *p = lcd_buffer + (x >> 3U) + y * LCD_WIDTH/8;
  int bit = x & 7;
  return *p & (1 << bit);
}

unsigned int get_keyboard_status(void)
{
  if (long_delay)
  {
    long_delay--;
    if (!long_delay)
    {
      unsigned int state = long_press ? long_press : key;
      long_press = 0;
      return state;
    }
  }
  return 0;
}

void power_off(void)
{

}

int capacitor_is_discharged(void)
{
  return 1;
}
