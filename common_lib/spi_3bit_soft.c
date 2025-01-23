#include "board.h"
#include <spi_3bit_soft.h>

unsigned int spi_3bit_send32(unsigned int data)
{
  unsigned long long int data_in = 0;
  SPI_3BIT_CLK_CLR;
  SPI_3BIT_CS_CLR;
  for (int i = 0; i < 11; i++)
  {
    SPI_3BIT_DATA_SET(data);
    data <<= 3;
    SPI_3BIT_CLK_SET;
    data_in <<= 3;
    SPI_3BIT_CLK_CLR;
    data_in |= SPI_3BIT_DATA_GET;
  }
  SPI_3BIT_CS_SET;
  data_in >>= 1;
  return (unsigned int)data_in;
}
