#include "pwm_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>

static int pwm_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem pwm_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
{NULL, NULL, pwm_handler}
};
static const ShellCommand pwm_command = {
  pwm_command_items,
  "pwm",
  NULL
};

static int pwm_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int frequency = atoi(argv[0]);
  int duty = atoi(argv[1]);
  pwm_set_frequency_and_duty(frequency * 1000, duty);
  return 0;
}

void register_pwm_commands(void)
{
  shell_register_command(&pwm_command);
}
