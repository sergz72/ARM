#include "tm1638_commands.h"
#include <read_hex_string.h>
#include <shell.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <tm1638.h>
#include <seven_seg.h>

static int set_led_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem set_led_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, set_led_handler}
};
static const ShellCommand set_leds_command = {
    set_led_command_items,
    "tm1638_led",
    "tm1638_led pos led_state"
};

static int out_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem out_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, out_handler}
};
static const ShellCommand out_command = {
    out_command_items,
    "tm1638_out",
    "tm1638_out pos text"
};

static int in_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem in_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, in_handler}
};
static const ShellCommand in_command = {
    in_command_items,
    "tm1638_in",
    "tm1638_in num_bytes"
};

static int display_on_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem display_on_command_items[] = {
    {NULL, NULL, display_on_handler}
};
static const ShellCommand display_on_command = {
    display_on_command_items,
    "tm1638_display_on",
    NULL
};

static int display_off_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem display_off_command_items[] = {
    {NULL, NULL, display_off_handler}
};
static const ShellCommand display_off_command = {
    display_off_command_items,
    "tm1638_display_off",
    NULL
};

static int brightness_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem brightness_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, brightness_handler}
};
static const ShellCommand brightness_command = {
    brightness_command_items,
    "tm1638_brightness",
    "tm1638_brightness value"
};

static int key_state_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem key_state_command_items[] = {
    {NULL, NULL, key_state_handler}
};
static const ShellCommand key_state_command = {
    key_state_command_items,
    "tm1638_key_state",
    NULL
};

static int set_led_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int pos, data;

  pos = atoi(argv[0]);
  if (pos < 0 || pos > 7)
  {
    pfunc("Invalid pos\n");
    return 1;
  }

  data = atoi(argv[1]);
  if (data < 0 || data > 1)
  {
    pfunc("Invalid data\n");
    return 1;
  }

  tm1638_set_led(pos, data);

  return 0;
}

static int out_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int pos, i, l;
  unsigned char encoded[8];

  pos = atoi(argv[0]);
  if (pos < 0 || pos > 7)
  {
    pfunc("Invalid pos\n");
    return 1;
  }
  l = (int)strlen(argv[1]);
  if (l + pos > 8)
  {
    pfunc("Text is too long\n");
    return 1;
  }

  for (i = 0; i < l; i++)
    encoded[i] = seven_seg_get_symbol(argv[1][i]);

  tm1638_out(pos, l, encoded);

  return 0;
}

static int in_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int num_bytes = atoi(argv[0]);
  unsigned int key_state;

  if (num_bytes <= 0 || num_bytes > 4)
  {
    pfunc("Invalid num_bytes\n");
    return 1;
  }

  key_state = tm1638_in(num_bytes);
  pfunc("key_state: %08X\n", key_state);

  return 0;
}

static int display_on_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  tm1638_display_on();
  return 0;
}

static int display_off_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  tm1638_display_off();
  return 0;
}

static int brightness_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int brightness = atoi(argv[0]);

  if (brightness <= 0 || brightness > 8)
  {
    pfunc("Invalid brightness\n");
    return 1;
  }
  tm1638_brightness(brightness - 1);
  return 0;
}

static int key_state_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int key_state = tm1638_key_state();

  pfunc("key_state: %08X\n", key_state);

  return 0;
}

void register_tm1638_commands(void)
{
  shell_register_command(&set_leds_command);
  shell_register_command(&out_command);
  shell_register_command(&in_command);
  shell_register_command(&display_on_command);
  shell_register_command(&display_off_command);
  shell_register_command(&brightness_command);
  shell_register_command(&key_state_command);
}
