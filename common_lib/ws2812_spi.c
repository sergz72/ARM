#include "board.h"
#include <ws2812_spi.h>

#define BYTES_PER_LED 9

static unsigned char ws2812_buffer[WS2812_MAX_LEDS * BYTES_PER_LED];
static unsigned char *ws2812_buffer_p;

static void prepare_buffer(unsigned char color)
{
  union
  {
    unsigned long i;
    unsigned char c[4];
  } data;

  data.i = 0;
  for (int i = 0; i < 8; i++)
  {
    data.i <<= 3;
    if (color & 0x80)
      data.i |= 6;
    else
      data.i |= 4;
    color <<= 1;
  }
  *ws2812_buffer_p++ = data.c[2];
  *ws2812_buffer_p++ = data.c[1];
  *ws2812_buffer_p++ = data.c[0];
}

void ws2812_send(int channel, const ws2812_rgb *data, unsigned int count)
{
  if (count == 0)
    return;
  if (count > WS2812_MAX_LEDS)
    count = WS2812_MAX_LEDS;
  ws2812_buffer_p = ws2812_buffer;
  unsigned int num_bytes = count * BYTES_PER_LED;
  while (count--)
  {
    prepare_buffer(data->green);
    prepare_buffer(data->red);
    prepare_buffer(data->blue);
    data++;
  }
  ws2812_spi_send(channel, ws2812_buffer, num_bytes);
}
