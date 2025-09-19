#include "board.h"
#include "adc_commands.h"
#include <shell.h>
#include <mcp3421.h>
#include <stdlib.h>
#include <systick.h>

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "adc_read",
  "adc_read gain",
  NULL,
  NULL
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int gain = atoi(argv[0]);
  if (gain < 0 || gain > 3)
    return 1;

  MCP3421Config dcfg = {
    .start_conversion = 1,
    .continuous_conversion = 0,
    .sample_rate = MCP3421_RATE_4,
    .gain = gain
  };
  int rc = mcp3421SetConfig(0, MCP3421_DEVICE_ID>>1, &dcfg);
  if (rc)
    return rc;

  delayms(300);

  int value;
  rc = mcp3421Get18BitVoltage(0, MCP3421_DEVICE_ID>>1, &value);
  if (rc)
    return rc;

  pfunc("result = %08X\r\n", value);

  return 0;
}

void register_adc_commands(void)
{
  shell_register_command(&read_command);
}
