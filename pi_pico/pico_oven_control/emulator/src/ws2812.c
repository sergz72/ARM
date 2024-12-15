#include "board.h"

void redraw_ws2812(void);

double ws2812_red, ws2812_green, ws2812_blue;

void put_pixel(unsigned int pixel_grb)
{
  ws2812_blue = (pixel_grb & 0xFF) / 25.0;
  ws2812_red = ((pixel_grb >> 8) & 0xFF) / 25.0;
  ws2812_green = ((pixel_grb >> 16) & 0xFF) / 25.0;
  redraw_ws2812();
}

unsigned int urgb_u32(unsigned int r, unsigned int g, unsigned int b)
{
    return (r << 8) | (g << 16) | b;
}

void WS2812Init(void)
{
  ws2812_red = ws2812_green = ws2812_blue = 0;
}
