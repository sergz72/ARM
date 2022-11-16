#include "board.h"
#include <spi_soft.h>

#ifndef SPI_SOFT_ONLY_SPI_COMMAND
void spi_delay(void)
{
  int i = SPI_DELAY;
  while (i--);
}

unsigned char spi_byte(unsigned char data, int count)
{
  int i;
  unsigned char out_data = 0;

  for (i = 0; i < 8; i++)
  {
    out_data <<= 1;
    if (data & 0x80)
      SPI_MOSI_SET; //set MOSI
    else
      SPI_MOSI_CLR; // clr MOSI
    spi_delay();
    if (SPI_CHECK_MISO) //check MISO
      out_data |= 1;
    SPI_CLK_SET; // set CLK
    if (count || i != 7)
    {
      spi_delay();
      SPI_CLK_CLR; // clr CLK
      data <<= 1;
    }
  }

  return out_data;
}
#endif

void spi_command(unsigned char cmd, unsigned char *data_in, unsigned char *data_out, int count, int set_cs)
{
  char data;

  SPI_CS_CLR; // clear CS
  SPI_CLK_CLR;
  data = spi_byte(cmd, count);
  count--;
  if (data_out)
    *data_out++ = data;
  while (count)
  {
    count--;
    data = spi_byte(data_in ? *data_in++ : 0, count);
    if (data_out)
      *data_out++ = data;
  }
  if (set_cs)
    SPI_CS_SET; // set CS
}
