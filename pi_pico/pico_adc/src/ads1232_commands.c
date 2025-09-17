#include "board.h"
#include "ads1232_commands.h"
#include <shell.h>
#include <stdlib.h>
#include <ads1232.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "ads1232_init",
  "ads1232_init",
  NULL,
  NULL
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ads1232_read",
  "ads1232_read channel gain",
  NULL,
  NULL
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_cpha1();
  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 3)
    return 1;
  int gain = atoi(argv[1]);
  if (gain < 0 || gain > 3)
    return 1;
  ads1232_set_channel(0, channel);
  ads1232_set_gain(0, gain);
  int value;
  int rc = ads1232_read(0, &value, ADS1232_TIMEOUT);
  if (rc)
    return rc;
  pfunc("result = %08X\r\n", value);
  return 0;
}

void register_ads1232_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&read_command);
}
