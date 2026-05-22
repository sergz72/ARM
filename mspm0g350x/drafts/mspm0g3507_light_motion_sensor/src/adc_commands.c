#include "adc_commands.h"
#include <shell.h>
#include "pir_sensor.h"

static int get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem get_command_items[] = {
  {nullptr, nullptr, get_handler}
};
static const ShellCommand get_command = {
  get_command_items,
  "adc_get",
  "adc_get",
  nullptr,
  nullptr
};

static int get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("filter_output=%d avg=%d\n", pir_filter_output, pir_adc_samples_avg);
  return 0;
}

void register_adc_commands(void)
{
  shell_register_command(&get_command);
}
