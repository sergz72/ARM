#ifndef _WS2812_H
#define _WS2812_H

#define WS2812_PIN 16

void WS2812Init(void);
void put_pixel(unsigned int pixel_grb);
unsigned int urgb_u32(unsigned int r, unsigned int g, unsigned int b);

#endif
