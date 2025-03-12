#include "board.h"
#include <spi_soft.h>

#ifndef SPI_SOFT_ONLY_SPI_COMMAND
void spi_delay(int channel)
{
  int i = SPI_DELAY(channel);
  while (i--);
}

unsigned char spi_byte(int channel, unsigned char data, int count)
{
  int i;
  unsigned char out_data = 0;

  for (i = 0; i < 8; i++)
  {
    out_data <<= 1;
    if (data & 0x80)
      SPI_MOSI_SET(channel); //set MOSI
    else
      SPI_MOSI_CLR(channel); // clr MOSI
    spi_delay(channel);
    if (SPI_CHECK_MISO(channel)) //check MISO
      out_data |= 1;
    SPI_CLK_SET(channel); // set CLK
#ifndef SPI_SOFT_CLK_IDLE_LOW
    if (count || i != 7)
    {
#endif
      spi_delay(channel);
      SPI_CLK_CLR(channel); // clr CLK
      data <<= 1;
#ifndef SPI_SOFT_CLK_IDLE_LOW
    }
#endif
  }

  return out_data;
}
#endif

void spi_command(int channel, unsigned char cmd, unsigned char *data_in, unsigned char *data_out, int count, int set_cs)
{
  char data;

  SPI_CLK_CLR(channel);
  SPI_CS_CLR(channel); // clear CS
  data = spi_byte(channel, cmd, count);
  if (data_out)
    *data_out++ = data;
  while (count)
  {
    count--;
    data = spi_byte(channel, data_in ? *data_in++ : 0, count);
    if (data_out)
      *data_out++ = data;
  }
  if (set_cs)
    SPI_CS_SET(channel); // set CS
}
