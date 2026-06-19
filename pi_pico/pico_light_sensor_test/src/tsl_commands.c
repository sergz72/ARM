#include "board.h"
#include <tsl_commands.h>
#include <shell.h>
#include <string.h>
#include <tsl2591.h>
#include <read_hex_string.h>
#include <stdlib.h>

static const tsl2591_config config = {
  .integration_time_ms = 600,
  .als_interrupt_enable = 0,
  .als_thresholds = {0,0},
  .no_persist_interrupt_enable = 1,
  .no_persist_als_thresholds = {1, 65535},
  .persistence_filter = AnyOutOfRange,
  .sleep_after_interrupt = 0
};

static int tsl_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tsl_init_command_items[] = {
  {NULL, NULL, tsl_init_handler}
};
static const ShellCommand tsl_init_command = {
  tsl_init_command_items,
  "tsl_init",
  "tsl_init",
  NULL,
  NULL
};

static int tsl_thr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tsl_thr_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, tsl_thr_handler}
};
static const ShellCommand tsl_thr_command = {
  tsl_thr_command_items,
  "tsl_thr",
  "tsl_thr als|nop low high",
  NULL,
  NULL
};

static int tsl_filter_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tsl_filter_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, tsl_filter_handler}
};
static const ShellCommand tsl_filter_command = {
  tsl_filter_command_items,
  "tsl_filter",
  "tsl_filter value",
  NULL,
  NULL
};

static int tsl_enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tsl_enable_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, tsl_enable_handler}
};
static const ShellCommand tsl_enable_command = {
  tsl_enable_command_items,
  "tsl_enable",
  "tsl_enable value",
  NULL,
  NULL
};

static int tsl_clr_int_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tsl_clr_int_command_items[] = {
  {NULL, NULL, tsl_clr_int_handler}
};
static const ShellCommand tsl_clr_int_command = {
  tsl_clr_int_command_items,
  "tsl_clr_int",
  "tsl_clr_int",
  NULL,
  NULL
};

static int tsl_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tsl_get_command_items[] = {
  {NULL, NULL, tsl_get_handler}
};
static const ShellCommand tsl_get_command = {
  tsl_get_command_items,
  "tsl_get",
  "tsl_get",
  NULL,
  NULL
};

static int tsl_status_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tsl_status_command_items[] = {
  {NULL, NULL, tsl_status_handler}
};
static const ShellCommand tsl_status_command = {
  tsl_status_command_items,
  "tsl_status",
  "tsl_status",
  NULL,
  NULL
};

static int tsl_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  tsl2591_result result;
  int rc = tsl2591_measure(&result);
  if (rc)
    return rc;
  pfunc("luminocity: %f gain %d tries %d\n", result.lux, result.gain, result.tries);
  return 0;
}

static int tsl_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return tsl2591_init(&config);
}

static int tsl_status_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned char status;
  int rc = tsl2591_get_status(&status);
  if (rc)
    return rc;
  pfunc("status: 0x%02X\r\n", status);
  return 0;
}

static int tsl_thr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  tsl2591_thresholds thresholds;
  
  int als = !strcmp(argv[0], "als");
  if (!als && strcmp(argv[0], "nop"))
  {
    pfunc("invalid threshold type\r\n");
    return 1; 
  }
  int low = atoi(argv[1]);
  int high = atoi(argv[2]);
  if (low < 0 || low > 65535 || high < 0 || high > 65535)
  {
    pfunc("invalid thresholds\r\n");
    return 1;
  }
  thresholds.low = low;
  thresholds.high = high;
  return als ? tsl2591_set_als_thresholds(&thresholds) : tsl2591_set_no_persist_als_thresholds(&thresholds);
}

static int tsl_clr_int_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return tsl2591_clear_interrupts();
}

static int tsl_filter_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  if (value < 0 || value > 15)
  {
    pfunc("invalid filter value\r\n");
    return 1;
  }
  return tsl2591_set_persistence_filter(value);
}

static int tsl_enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned char value;

  int rc = read_hex_string(argv[0], &value,1);
  if (rc <= 0)
  {
    pfunc("Invalid value\r\n");
    return 2;
  }
  return tsl2591_enable(value);
}

void register_tsl_commands(void)
{
  shell_register_command(&tsl_init_command);
  shell_register_command(&tsl_get_command);
  shell_register_command(&tsl_thr_command);
  shell_register_command(&tsl_clr_int_command);
  shell_register_command(&tsl_filter_command);
  shell_register_command(&tsl_status_command);
  shell_register_command(&tsl_enable_command);
}
