#include "board.h"
#include <veml_commands.h>
#include <shell.h>
#include <veml7700.h>

static int veml_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem veml_init_command_items[] = {
  {NULL, NULL, veml_init_handler}
};
static const ShellCommand veml_init_command = {
  veml_init_command_items,
  "veml_init",
  "veml_init",
  NULL,
  NULL
};

static int veml_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem veml_get_command_items[] = {
  {NULL, NULL, veml_get_handler}
};
static const ShellCommand veml_get_command = {
  veml_get_command_items,
  "veml_get",
  "veml_get",
  NULL,
  NULL
};

static int veml_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  veml7700_result result;
  for (int i = 0; i < 5; i++)
  {
    int rc = veml7700_measure(&result, 1);
    if (rc)
      return rc;
    pfunc("luminocity: %f gainx8 %d tries %d\n", result.lux, result.gainx8, result.tries);
  }
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
