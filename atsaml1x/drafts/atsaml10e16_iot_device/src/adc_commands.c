#include "board.h"
#include "adc_commands.h"
#include <shell.h>

static int vcc_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem vcc_command_items[] = {
  {nullptr, nullptr, vcc_handler}
};
static const ShellCommand vcc_command = {
  vcc_command_items,
  "vcc",
  "vcc",
  nullptr,
  nullptr
};

static int vcc_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("vcc=%dmV\n", get_vcc());
  return 0;
}

void register_adc_commands(void)
{
  shell_register_command(&vcc_command);
}
