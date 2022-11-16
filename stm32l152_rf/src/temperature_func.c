#include <board.h>
#include <st7066.h>
#include <temperature_func.h>

void showTemperature(int temperature)
{
  unsigned int v;

  if (temperature < 0)
  {
    st7066_write_data('-');
    temperature = -temperature;
  }
  else
    st7066_write_data(' ');

  v = temperature / 100;
  if (!v)
    st7066_write_data(' ');
  else
    st7066_write_data('0' + v);

  v = (temperature / 10) % 10;
  st7066_write_data('0' + v);

  st7066_write_data('.');

  v = temperature % 10;
  st7066_write_data('0' + v);
}
