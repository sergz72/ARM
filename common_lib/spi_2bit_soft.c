#include "board.h"
#include <spi_3bit_soft.h>

unsigned int spi_3bit_send32(unsigned int data)
{
  unsigned int data_in = 0;
  SPI_2BIT_CLK_CLR;
  SPI_2BIT_CS_CLR;
  for (int i = 0; i < 16; i++)
  {
    SPI_2BIT_DATA_SET(data);
    data <<= 2;
    SPI_2BIT_CLK_SET;
    data_in <<= 2;
    SPI_2BIT_CLK_CLR;
    data_in |= SPI_2BIT_DATA_GET;
  }
  SPI_2BIT_CS_SET;
  return data_in;
}
