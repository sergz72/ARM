#include "board.h"
#include "th02.h"
#include <st7066.h>
#include <temperature_func.h>

int prev_temperature;
unsigned int prev_humi;

unsigned int TH02Start(void)
{
  prev_temperature = -1000;
  prev_humi = 101;

  st7066_set_ddram_address(0x40);
  st7066_write_data('I');
  st7066_write_data('n');
  st7066_set_ddram_address(0x4B);
  st7066_write_data('H');
  st7066_write_data('u');
  st7066_write_data('m');
  st7066_write_data('i');
  st7066_write_data('d');
  st7066_set_ddram_address(0x53);
  st7066_write_data('%');

  return th02_check();
}

static void showTemp(unsigned int rc, int temperature)
{
  if (rc && prev_temperature == temperature)
    return;

  prev_temperature = temperature;

  st7066_set_ddram_address(0x44);

  if (!rc || (temperature > 999) || (temperature < -999))
  {
    st7066_write_data(' ');
    st7066_write_data('E');
    st7066_write_data('r');
    st7066_write_data('r');
    st7066_write_data(' ');
    return;
  }

  showTemperature(temperature);
}

static void showHumi(unsigned int rc, unsigned int humi)
{
  unsigned int v;

  if (rc && prev_humi == humi)
    return;

  prev_humi = humi;

  st7066_set_ddram_address(0x51);

  if (!rc || (humi > 99))
  {
    st7066_write_data('E');
    st7066_write_data('r');
    return;
  }
  v = humi / 10;
  if (!v)
    st7066_write_data(' ');
  else
    st7066_write_data('0' + v);

  v = humi % 10;
  st7066_write_data('0' + v);
}

void TH02Loop(unsigned int rc)
{
  int temperature;
  unsigned int humidity;
  if (rc)
    rc = th02_get_temperature(&temperature);
  if (rc)
    rc = th02_get_humidity(&humidity);
  showTemp(rc, temperature);
  showHumi(rc, humidity);
}