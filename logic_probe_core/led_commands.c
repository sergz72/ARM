#include "led_commands.h"
#include "board.h"
#include "ws2812_spi.h"
#include <shell.h>
#include <read_hex_string.h>
#include <ui.h>

static int leds_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem leds_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
{NULL, NULL, leds_handler}
};
static const ShellCommand leds_command = {
  leds_command_items,
  "leds",
  "leds rgb1 rgb2 rgb3 rgb4",
};

static void set_led_data(long *prgb)
{
  for (int i = 0; i < WS2812_MAX_LEDS; i++)
  {
    long rgb = *prgb++;
    led_data[i].red = (unsigned char)rgb;
    led_data[i].green = (unsigned char)(rgb >> 8);
    led_data[i].blue = (unsigned char)(rgb >> 16);
  }
}

static int leds_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  long rgb[4] = {0,0,0,0};
  
  for (int i = 0; i < WS2812_MAX_LEDS; i++)
  {
    read_hex_string(argv[i], (unsigned char*)&rgb[i], 3);
    pfunc("bgr%d=0x%lx\n", i, rgb[i]);
  }
  
  
  set_led_data(rgb);
  
  ws2812_send(0, (const ws2812_rgb *)&led_data, WS2812_MAX_LEDS);
  
  return 0;
}

void register_led_commands(void)
{
  shell_register_command(&leds_command);
}
