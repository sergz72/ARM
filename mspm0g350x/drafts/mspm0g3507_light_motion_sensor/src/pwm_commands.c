#include "pwm_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>
#include <string.h>

static int pwm_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int pwm2_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem pwm_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, pwm2_handler},
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

static int pwm2_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (!strcmp(argv[0], "on"))
  {
    pwm_on();
    pfunc("PWM is on\n");
  }
  else if (!strcmp(argv[0], "off"))
  {
    pwm_off();
    pfunc("PWM is off\n");
  }
  else
    return 1;
  return 0;
}

void register_pwm_commands(void)
{
  shell_register_command(&pwm_command);
}
