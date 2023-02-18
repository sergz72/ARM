#include "max7219_commands.h"
#include <shell.h>
#include <max7219.h>
#include <seven_seg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pico/time.h>

static int out_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem out_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, out_handler}
};
static const ShellCommand out_command = {
    out_command_items,
    "max7219_out",
    "max7219_out text"
};

static int display_on_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem display_on_command_items[] = {
    {NULL, NULL, display_on_handler}
};
static const ShellCommand display_on_command = {
    display_on_command_items,
    "max7219_display_on",
    NULL
};

static int display_off_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem display_off_command_items[] = {
    {NULL, NULL, display_off_handler}
};
static const ShellCommand display_off_command = {
    display_off_command_items,
    "max7219_display_off",
    NULL
};

static int brightness_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem brightness_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, brightness_handler}
};
static const ShellCommand brightness_command = {
    brightness_command_items,
    "max7219_brightness",
    "max7219_brightness value"
};

static int test_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem test_command_items[] = {
    {NULL, NULL, test_handler}
};
static const ShellCommand test_command = {
    test_command_items,
    "max7219_test",
    NULL
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
    {NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
    init_command_items,
    "max7219_init",
    NULL
};

static int out_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int i, l;
  unsigned char encoded[32];

  l = (int)strlen(argv[0]);
  if (l > 32)
  {
    pfunc("Text is too long\n");
    return 1;
  }

  for (i = 0; i < 32; i++)
  {
    if (i < l)
      encoded[i] = seven_seg_get_symbol(argv[0][i]);
    else
      encoded[i] = 0;
  }

  max7219_out(encoded);

  return 0;
}

static int display_on_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  max7219_on();
  return 0;
}

static int display_off_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  max7219_off();
  return 0;
}

static int test_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  max7219_display_test_on();
  sleep_ms(3000);
  max7219_display_test_off();
  return 0;
}

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  max7219_init(1);
  return 0;
}

static int brightness_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int brightness = atoi(argv[0]);

  if (brightness < 0 || brightness > 15)
  {
    pfunc("Invalid brightness\n");
    return 1;
  }
  max7219_intensity(brightness);
  return 0;
}

void register_max7219_commands(void)
{
  shell_register_command(&out_command);
  shell_register_command(&display_on_command);
  shell_register_command(&display_off_command);
  shell_register_command(&brightness_command);
  shell_register_command(&test_command);
  shell_register_command(&init_command);
}
