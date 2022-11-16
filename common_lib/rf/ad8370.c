#include <ad8370.h>
#include <board.h>

void AD8370_SetGain(int spi_id, float gain)
{
  unsigned char code;

//AV = GainCode * 0.055744 * (1 + 6.079458 * MSB)

  if (gain < 0)
    gain = 0;
  if (gain > AD8370_MAX_GAIN)
    gain = AD8370_MAX_GAIN;
  if (gain <= 7.079488)
  {
    code = (int)(gain / AD8370_MIN_GAIN);
    if (code > 127)
      code = 127;
  }
  else
  {
    code = (int)(gain / 0.394637306752);
    if (code > 127)
      code = 127;
    code |= 0x80;
  }
  ad8370_spi_send(spi_id, code);
}
