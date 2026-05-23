#include "adc_commands.h"
#include <shell.h>

#include "board.h"
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

static int get_vbat_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem get_vbat_command_items[] = {
  {nullptr, nullptr, get_vbat_handler}
};
static const ShellCommand get_vbat_command = {
  get_vbat_command_items,
  "adc_get_vbat",
  "adc_get_vbat",
  nullptr,
  nullptr
};

static int get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("filter_output=%d avg=%d\n", pir_filter_output, pir_adc_samples_avg);
  return 0;
}

static int get_vbat_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int vbat_raw = get_vbat();
  unsigned int vbat = vbat_raw*VREF_VOLTAGE_MV*3/4096;
  pfunc("%d %d\n", vbat_raw, vbat);
  return 0;
}

void register_adc_commands(void)
{
  shell_register_command(&get_command);
  shell_register_command(&get_vbat_command);
}
