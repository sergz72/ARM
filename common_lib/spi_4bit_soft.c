#include "board.h"
#include <spi_4bit_soft.h>
//#include <stdio.h>

/*
 * data is in format
 * data stream 0 pointer
 * data stream 1 pointer
 * ....
 */
void spi_4bit_send16(unsigned short **data, int data_length, int count)
{
  int i, j;
  unsigned short words[data_length];
  unsigned char d;

  SPI_4BIT_CLK_CLR;
  SPI_4BIT_CS_CLR; // clear CS
  while (count--)
  {
    for (i = 0; i < data_length; i++)
    {
      words[i] = *data[i];
      data[i]++;
    }
    for (i = 0; i < 16; i++)
    {
      d = 0;
      for (j = 0; j < data_length; j++)
      {
        d <<= 1;
        if (words[j] & 0x8000)
          d |= 1;
        words[j] <<= 1;
      }
      //printf("data=%02d\n", d);
      SPI_4BIT_DATA_SET(d);
      spi_4bit_delay();
      SPI_4BIT_CLK_SET; // set CLK
      spi_4bit_delay();
      SPI_4BIT_CLK_CLR; // clr CLK
    }
  }
  spi_4bit_delay();
  SPI_4BIT_CS_SET; // set CS
}
