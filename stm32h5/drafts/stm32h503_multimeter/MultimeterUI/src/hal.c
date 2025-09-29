#include "board.h"
#include <lcd_sh1107.h>
#include <ad7793.h>
#include <ads1220.h>
#include <string.h>
#include "multimeter.h"

#define SIZE (LCD_HEIGHT * LCD_WIDTH / 8)

extern int long_delay;
extern unsigned int long_press;
extern unsigned int key;

static unsigned char lcd_buffer[SIZE], *lcd_buffer_p = lcd_buffer;

volatile int capacity_measurement_done;

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

unsigned int get_capacity_measurement_start_time(void)
{
  return 0;
}

unsigned int get_capacity_measurement_end_time(void)
{
  return 0;
}

void capacity_measurement_start(int channel)
{
  discharge_off();
  charge_on(channel);
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

void ad7793_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
}

void ads1220_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
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

unsigned int check_measurements_statuses(void)
{
  return 0;
}

void start_voltage_measurements(void)
{

}

void start_frequency_measurement(void)
{

}

void start_resistance_measurement(int channel, enum resistance_measurements_modes mode)
{

}

void start_capacitance_measurement(int channel)
{

}

void start_current_measurement(int channel)
{

}

unsigned int start_extra_measurements(int channel, int extra_measurement_no)
{
  return 0;
}

unsigned int finish_voltage_measurement(int channel)
{
  return 0;
}

unsigned int finish_frequency_measurement(void)
{
  return 0;
}

unsigned int finish_resistance_measurement(int channel, int mode)
{
  return 0;
}

unsigned int finish_current_measurement(int channel)
{
  return 0;
}

void finish_capacitance_measurement(void)
{

}

unsigned int finish_capacitance_measurement_1k(void)
{
  return 0;
}

unsigned int finish_temperature_measurement(void)
{
  return 0;
}

unsigned int finish_vdda_measurement(void)
{
  return 0;
}
