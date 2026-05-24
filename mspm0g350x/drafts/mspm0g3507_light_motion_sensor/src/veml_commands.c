#include "board.h"
#include <veml_commands.h>
#include <shell.h>
#include <veml7700.h>

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
  unsigned short result;
  int rc = veml7700_read(VEML7700_REG_ALS, &result);
  if (rc)
    return rc;
  unsigned int lux = VEML7700_LUX_X100(result);
  bool high_th_alert = result > VEML7700_HIGH_THRESHOLD;
  bool low_th_alert = result < VEML7700_LOW_THRESHOLD;
  pfunc("luminosity: %d.%02d, high alert: %d, low alert: %d\n", lux / 100, lux % 100, high_th_alert, low_th_alert);
  return 0;
}

void register_veml_commands(void)
{
  shell_register_command(&veml_get_command);
}
