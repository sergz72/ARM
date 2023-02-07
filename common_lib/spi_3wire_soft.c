#include "board.h"
#include <spi_3wire_soft.h>

#ifndef SPI3_SOFT_ONLY_SPI_COMMAND
#ifndef SPI3_CUSTOM_DELAY
void spi3_delay(void)
{
  int i = SPI3_DELAY;
  while (i--);
}
#endif

#ifdef SPI3_LSB_FIRST
#define DATA_AND 1
#define DATA_OR 0x80
#else
#define DATA_AND 0x80
#define DATA_OR 1
#endif

void spi3_byte_out(unsigned char data, int count)
{
  int i;

  for (i = 0; i < 8; i++)
  {
    if (data & DATA_AND)
      SPI3_DIO_SET; //set DIO
    else
      SPI3_DIO_CLR; // clr DIO
    spi3_delay();
    SPI3_CLK_SET; // set CLK
    if (count || i != 7)
    {
      spi3_delay();
      SPI3_CLK_CLR; // clr CLK
#ifdef SPI3_LSB_FIRST
      data >>= 1;
#else
      data <<= 1;
#endif
    }
  }
}

unsigned char spi3_byte_in(int count)
{
  int i;
  unsigned char out_data = 0;

  SPI3_DIO_SET; //set DIO

  for (i = 0; i < 8; i++)
  {
#ifdef SPI3_LSB_FIRST
    out_data >>= 1;
#else
    out_data <<= 1;
#endif
    spi3_delay();
    if (SPI3_CHECK_DIO) //check DIO
      out_data |= DATA_OR;
    SPI3_CLK_SET; // set CLK
    if (count || i != 7)
    {
      spi3_delay();
      SPI3_CLK_CLR; // clr CLK
    }
  }

  return out_data;
}

#endif

void spi3_command(unsigned char cmd, unsigned char *data_out, int count_out, unsigned char *data_in, int count_in, int set_cs, int data_in_delay)
{
  SPI3_CS_CLR; // clear CS
  SPI3_CLK_CLR;
  spi3_byte_out(cmd, count_out + count_in);
  if (data_out)
  {
    while (count_out--)
      spi3_byte_out(*data_out++, count_out + count_in);
  }
  if (data_in)
  {
    while (data_in_delay--)
      spi3_delay();
    while (count_in--)
      *data_in++ = spi3_byte_in(count_in);
  }
  if (set_cs)
    SPI3_CS_SET; // set CS
}
