#include "board.h"
#include "ina226_commands.h"
#include <shell.h>
#include <ina226.h>

const INA226Config cfg = {
  .awg = INA226_AVG_1,
  .vbusct = INA226_VBUSCT_1100,
  .vshct = INA226_VSHCT_1100,
  .mode = INA226_MODE_SHUNT_BUS_TRIG
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
{NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ina226_read",
  "ina226_read"
};

static int reset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem reset_command_items[] = {
  {NULL, NULL, reset_handler}
};
static const ShellCommand reset_command = {
  reset_command_items,
  "ina226_reset",
  "ina226_reset"
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc = ina226SetConfig(0, INA_ADDRESS, &cfg);
  if (rc != 0)
  {
    pfunc("ina226SetConfig failed\n");
    return rc;
  }
  delayms(3);
  int value;
  rc = ina226GetBusVoltage(0, INA_ADDRESS, &value);
  if (rc != 0)
  {
    pfunc("ina226GetBusVoltage failed\n");
    return rc;
  }
  pfunc("Bus voltage %d mV\n", value);
  rc = ina226GetShuntCurrent(0, INA_ADDRESS, 100, &value);
  if (rc != 0)
  {
    pfunc("ina226GetShuntCurrent failed\n");
    return rc;
  }
  pfunc("Shunt current %d uA\n", value);
  return 0;
}

static int reset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return ina226Reset(0, INA_ADDRESS);
}

void register_ina226_commands(void)
{
  shell_register_command(&reset_command);
  shell_register_command(&read_command);
}
