#include "board.h"
#include "ina228_commands.h"
#include <shell.h>
#include <ina228.h>

static const INA228Config cfg1 = {
  .bits = {
    .adcrange = 1,
    .tempcomp = 0,
    .convdly = 0,
    .rstacc = 0,
    .rst = 0,
    .reserved = 0
  }
};

static const INA228Config cfg0 = {
  .bits = {
    .adcrange = 0,
    .tempcomp = 0,
    .convdly = 0,
    .rstacc = 0,
    .rst = 0,
    .reserved = 0
  }
};

static const INA228Config cfg_reset = {
  .bits = {
    .adcrange = 0,
    .tempcomp = 0,
    .convdly = 0,
    .rstacc = 0,
    .rst = 1,
    .reserved = 0
  }
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ina228_read",
  "ina228_read"
};

static int init1_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init1_command_items[] = {
  {NULL, NULL, init1_handler}
};
static const ShellCommand init1_command = {
  init1_command_items,
  "ina228_init1",
  "ina228_init1"
};

static int init0_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init0_command_items[] = {
  {NULL, NULL, init0_handler}
};
static const ShellCommand init0_command = {
  init0_command_items,
  "ina228_init0",
  "ina228_init0"
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value;
  int rc = ina228GetBusVoltage(0, INA_ADDRESS, &value);
  if (rc != 0)
  {
    pfunc("ina228GetBusVoltage failed\n");
    return rc;
  }
  pfunc("Bus voltage %d uV\n", value);
  rc = ina228GetShuntCurrent(0, INA_ADDRESS, 1000, &value);
  if (rc != 0)
  {
    pfunc("ina228GetShuntCurrent failed\n");
    return rc;
  }
  pfunc("Shunt current %d nA\n", value);
  rc = ina228GetTemperature(0, INA_ADDRESS, &value);
  if (rc != 0)
  {
    pfunc("ina228GetTemperature failed\n");
    return rc;
  }
  pfunc("Temperature %d.%03d grad\n", value / 1000, value % 1000);
  return 0;
}

static int init1_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc = ina228SetConfig(0, INA_ADDRESS, cfg_reset);
  if (rc != 0)
  {
    pfunc("ina228SetConfig_reset failed\n");
    return rc;
  }
  rc = ina228SetConfig(0, INA_ADDRESS, cfg1);
  if (rc != 0)
  {
    pfunc("ina228SetConfig failed\n");
    return rc;
  }
  return 0;
}

static int init0_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc = ina228SetConfig(0, INA_ADDRESS, cfg_reset);
  if (rc != 0)
  {
    pfunc("ina228SetConfig_reset failed\n");
    return rc;
  }
  rc = ina228SetConfig(0, INA_ADDRESS, cfg0);
  if (rc != 0)
  {
    pfunc("ina228SetConfig failed\n");
    return rc;
  }
  return 0;
}

void register_ina228_commands(void)
{
  shell_register_command(&init0_command);
  shell_register_command(&init1_command);
  shell_register_command(&read_command);
}
