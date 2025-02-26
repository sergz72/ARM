#include <ad8370.h>

#define DB_PER_CODE 3333 // * 10000
#define MAX_LOW_GAIN 17

int AD8370_SetGain(int spi_id, int gain)
{
  unsigned char code, gcode;

  if (gain < AD8370_MIN_GAIN || gain > AD8370_MAX_GAIN)
    return 1;

  if (gain > MAX_LOW_GAIN)
  {
    code = 0x80;
    gain -= 17;
  }
  else
    code = 0;
  
  gain -= AD8370_MIN_GAIN;

  gcode = (unsigned char)(gain * 10000 / DB_PER_CODE + 1);

  ad8370_spi_send(spi_id, code + gcode);

  return 0;
}
