#include <ad9850.h>
#include "board.h"

typedef struct
{
  unsigned char phase;
  unsigned char refclk_multiplier_enable;
  unsigned char parallel_load;
} AD9850_Channel;

static AD9850_Channel channels[AD9850_MAX_CHANNELS];

__attribute__((weak)) void ad9850_write(int channel, const unsigned char *data, unsigned int length)
{
  AD9850_FQUD_CLR(channel);
  AD9850_DELAY;
  while (length--)
  {
    AD9850_DATA_SET(channel, *data++);
    AD9850_DELAY;
    AD9850_CLK_SET(channel);
    AD9850_DELAY;
    AD9850_CLK_CLR(channel);
    AD9850_DELAY;
  }
  AD9850_FQUD_SET(channel);
}

void ad9850_init(int channel, unsigned char refclk_multiplier_enable, unsigned char parallel_load)
{
  channels[channel].phase = 0;
  channels[channel].refclk_multiplier_enable = refclk_multiplier_enable;
  channels[channel].parallel_load = parallel_load;
  ad9850_reset(channel);
}

void ad9850_reset(int channel)
{
  AD9850_RESET_SET(channel);
  if (channels[channel].parallel_load)
  {
    for (int i = 0; i < 8; i++)
    {
      AD9850_DELAY;
      AD9850_CLK_SET(channel);
      AD9850_DELAY;
      AD9850_CLK_CLR(channel);
    }
  }
  else
  {
    unsigned char data = 0;
    ad9850_write(channel, &data, 1);
  }
  AD9850_RESET_CLR(channel);
}

unsigned char build_control_word(int channel)
{
  return (channels[channel].phase << 3) | (channels[channel].refclk_multiplier_enable ? 1 : 0);
}

void ad9850_power_up(int channel)
{
  unsigned char data = build_control_word(channel);
  ad9850_write(channel, &data, 1);
}

void ad9850_power_down(int channel)
{
  unsigned char data = 0x04;
  ad9850_write(channel, &data, 1);
}

void ad9850_set_freq_word(int channel, unsigned long word, unsigned char phase)
{
  unsigned char bytes[5];
  channels[channel].phase = phase;
  if (channels[channel].parallel_load)
  {
    bytes[0] = build_control_word(channel);
    bytes[1] = word >> 24;
    bytes[2] = (word >> 16) & 0xFF;
    bytes[3] = (word >> 8) & 0xFF;
    bytes[4] = word & 0xFF;
  }
  else
  {
    bytes[0] = word & 0xFF;
    bytes[1] = (word >> 8) & 0xFF;
    bytes[2] = (word >> 16) & 0xFF;
    bytes[3] = word >> 24;
    bytes[4] = build_control_word(channel);
  }
  ad9850_write(channel, bytes, 5);
}
