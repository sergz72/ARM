#include "board.h"
#include "multimeter_commands.h"
#include <shell.h>
#include "multimeter.h"

static int c_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem c_command_items[] = {
  {NULL, NULL, c_handler}
};
static const ShellCommand c_command = {
  c_command_items,
  "c",
  "c",
  NULL,
  NULL
};

static int c_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  capacitance_result result;
  int rc = get_capacitance(&result);
  if (rc)
    return rc;
  pfunc("Result = %d pF, difference = %d, channel = %d\r\n", result.pF, result.diff, result.channel);
  return 0;
}

void register_multimeter_commands(void)
{
  shell_register_command(&c_command);
}
