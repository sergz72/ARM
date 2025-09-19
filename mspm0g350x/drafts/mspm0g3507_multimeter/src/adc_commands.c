#include "board.h"
#include "adc_commands.h"
#include <shell.h>
#include <stdlib.h>

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
  int value;
  int rc = adc_read(gain, &value);
  if (rc)
    return rc;

  pfunc("result = %d uV\r\n", value);

  return 0;
}

void register_adc_commands(void)
{
  shell_register_command(&read_command);
}
