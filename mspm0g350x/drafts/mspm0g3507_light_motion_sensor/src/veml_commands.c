#include "board.h"
#include <veml_commands.h>
#include <shell.h>
#include <veml7700.h>

static int veml_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem veml_init_command_items[] = {
  {nullptr, nullptr, veml_init_handler}
};
static const ShellCommand veml_init_command = {
  veml_init_command_items,
  "veml_init",
  "veml_init",
  nullptr,
  nullptr
};

static int veml_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem veml_get_command_items[] = {
  {nullptr, nullptr, veml_get_handler}
};
static const ShellCommand veml_get_command = {
  veml_get_command_items,
  "veml_get",
  "veml_get",
  nullptr,
  nullptr
};

static int veml_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  veml7700_result result;
  int rc = veml7700_measure(&result);
  if (rc)
    return rc;
  unsigned int lux = (unsigned int)(result.lux * 100);
  pfunc("luminocity: %d.%02d gainx8 %d tries %d\n", lux / 100, lux % 100, result.gainx8, result.tries);
  return 0;
}

static int veml_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return veml7700_init();
}

void register_veml_commands(void)
{
  shell_register_command(&veml_init_command);
  shell_register_command(&veml_get_command);
}
