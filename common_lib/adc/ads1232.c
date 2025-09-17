#include "board.h"
#include <ads1232.h>

void ads1232_set_gain(int channel, int gain)
{
  if (gain & 1)
    ADS1232_GAIN0_SET(channel);
  else
    ADS1232_GAIN0_CLR(channel);
  if (gain & 2)
    ADS1232_GAIN1_SET(channel);
  else
    ADS1232_GAIN1_CLR(channel);
}

void ads1232_set_channel(int channel, int channel_no)
{
  if (channel_no & 1)
    ADS1232_A0_SET(channel);
  else
    ADS1232_A0_CLR(channel);
  if (channel_no & 2)
    ADS1232_TEMP_SET(channel);
  else
    ADS1232_TEMP_CLR(channel);
}

static int ads1232_wait(int channel, int timeout)
{
  while (timeout)
  {
    delayms(1);
    if (!ADS1232_DRDY_GET(channel))
      break;
    timeout--;
  }
  return timeout == 0;
}

int ads1232_read(int channel, int *pvalue, int timeout)
{
  int rc = ads1232_wait(channel, timeout);
  if (rc)
    return rc;
  unsigned char data[3];
  ads1232_spi_transfer(channel, data);
  int value = (data[0] << 16) | (data[1] << 8) | data[2];
  if (value & 0x800000)
    value |= 0xFF000000;
  *pvalue = value;
  return 0;
}