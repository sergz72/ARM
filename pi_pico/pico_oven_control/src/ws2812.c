#include "board.h"
#include "ws2812.pio.h"

#define IS_RGBW false

void put_pixel(unsigned int pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

unsigned int urgb_u32(unsigned int r, unsigned int g, unsigned int b)
{
    return (r << 8) | (g << 16) | b;
}

void WS2812Init(void)
{
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
}
