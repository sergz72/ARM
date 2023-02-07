#include <ad9833.h>

void ad9833_set_config(int channel, unsigned short cfg)
{
  ad9833_write(channel, cfg|0x2000); // B28 is set
}

void ad9833_set_freq0_word(int channel, unsigned long word)
{
  ad9833_write(channel, 0x4000 | (word & 0x3FFF));
  ad9833_write(channel, 0x4000 | ((word >> 14) & 0x3FFF));
}

void ad9833_set_freq1_word(int channel, unsigned long word)
{
  ad9833_write(channel, 0x8000 | (word & 0x3FFF));
  ad9833_write(channel, 0x8000 | ((word >> 14) & 0x3FFF));
}

void ad9833_set_phase0(int channel, unsigned short phase)
{
  ad9833_write(channel, 0xC000 | (phase & 0xFFF));
}

void ad9833_set_phase1(int channel, unsigned short phase)
{
  ad9833_write(channel, 0xE000 | (phase & 0xFFF));
}

int ad9833_is_output_enabled(unsigned short cfg)
{
  return (cfg & (AD9833_CFG_SLEEP1 | AD9833_CFG_SLEEP12)) == 0;
}

void ad9833_update_mode(int channel, unsigned short *cfg, unsigned short new_mode)
{
  unsigned short c = (*cfg & 0xFFC0) | new_mode;
  *cfg = c;
  if (ad9833_is_output_enabled(c))
    ad9833_set_config(channel, c);
}
