#include "board.h"
#include <spi_soft.h>

#ifndef SPI_SOFT_ONLY_SPI_COMMAND
static int cpha_values[SPI_CHANNELS];

void spi_channel_init(int channel, int cpha)
{
  cpha_values[channel] = cpha;
}

void __weak spi_delay(int channel)
{
  int i = SPI_DELAY(channel);
  while (i--);
}

unsigned char spi_byte(int channel, unsigned char data)
{
  int i;
  unsigned char out_data = 0;
  int cpha = cpha_values[channel];

  for (i = 0; i < 8; i++)
  {
    out_data <<= 1;
    if (cpha)
    {
      spi_delay(channel);
      SPI_CLK_ACTIVE(channel);
    }
    if (data & 0x80)
      SPI_MOSI_SET(channel); //set MOSI
    else
      SPI_MOSI_CLR(channel); // clr MOSI
    spi_delay(channel);
    if (SPI_CHECK_MISO(channel)) //check MISO
      out_data |= 1;
    if (cpha)
      SPI_CLK_IDLE(channel);
    else
    {
      SPI_CLK_ACTIVE(channel);
      spi_delay(channel);
      SPI_CLK_IDLE(channel);
    }
    data <<= 1;
  }

  return out_data;
}
#endif

void spi_command(int channel, unsigned char cmd, unsigned char *data_in, unsigned char *data_out, int count, int set_cs)
{
  char data;

  SPI_CS_CLR(channel); // clear CS
  data = spi_byte(channel, cmd);
  if (data_out)
    *data_out++ = data;
  while (count)
  {
    count--;
    data = spi_byte(channel, data_in ? *data_in++ : 0);
    if (data_out)
      *data_out++ = data;
  }
  if (set_cs)
    SPI_CS_SET(channel); // set CS
}

void spi_transfer(int channel, unsigned char *data_in, unsigned char *data_out, int count, int set_cs)
{
  char data;

  SPI_CS_CLR(channel); // clear CS
  while (count)
  {
    count--;
    data = spi_byte(channel, data_in ? *data_in++ : 0);
    if (data_out)
      *data_out++ = data;
  }
  if (set_cs)
    SPI_CS_SET(channel); // set CS
}
