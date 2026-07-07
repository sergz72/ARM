#include "scd41_commands.h"
#include <shell.h>
#include <scd4x.h>
#include <delay_systick.h>

static int scd_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem scd_get_command_items[] = {
  {nullptr, nullptr, scd_get_handler}
};
static const ShellCommand scd_get_command = {
  scd_get_command_items,
  "scd_get",
  "scd_get",
  nullptr,
  nullptr
};

static int scd_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  scd4x_result result;
  scd4x_wake_up(); // no acknowledge for this command - no error checking
  int rc = scd4x_start_measurement();
  if (rc)
  {
    pfunc("scd4x_start_measurement failed\n");
    return rc;
  }
  delayms(6000);
  rc = scd4x_read_measurement(&result);
  if (rc)
  {
    pfunc("scd4x_read_measurement failed\n");
    return rc;
  }
  rc = scd4x_power_down();
  if (rc)
  {
    pfunc("scd4x_power_down failed\n");
    return rc;
  }
  pfunc("CO2: %d\ntemperature: %d\nhumidity: %d\n", result.co2, result.temperature, result.humidity);
  return 0;
}

void register_scd41_commands(void)
{
  shell_register_command(&scd_get_command);
}
