#include <ad9850.h>
#include "board.h"

static unsigned char channel_phase[AD9850_MAX_CHANNELS];

static void ad9850_write(int channel, unsigned char *data, unsigned int length)
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

void ad9850_init(void)
{
  int i;
  for (i = 0; i < AD9850_MAX_CHANNELS; i++)
  {
    channel_phase[i] = 0;
    ad9850_reset(i);
  }
}

void ad9850_reset(int channel)
{
  AD9850_RESET_SET(channel);
  AD9850_DELAY;
  AD9850_DELAY;
  AD9850_RESET_CLR(channel);
}

void ad9850_power_up(int channel)
{
  unsigned char data = channel_phase[channel] << 3;
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
  channel_phase[channel] = phase;
  bytes[0] = phase << 3;
  bytes[1] = word >> 24;
  bytes[2] = (word >> 16) & 0xFF;
  bytes[3] = (word >> 8) & 0xFF;
  bytes[4] = word & 0xFF;
  ad9850_write(channel, bytes, 5);
}
