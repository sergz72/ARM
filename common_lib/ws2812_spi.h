#ifndef WS2812_SPI_H
#define WS2812_SPI_H

typedef struct
{
  unsigned char green;
  unsigned char red;
  unsigned char blue;
} ws2812_rgb;

void ws2812_send(int channel, const ws2812_rgb *data, unsigned int count);
void ws2812_spi_send(int channel, const unsigned char *data, unsigned int count); // should be defined in hal.c

#endif
