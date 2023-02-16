#include "board.h"
#include <dev_freq_pwm.h>

void *freq_pwm_initializer(void)
{
  return NULL;
}

void *freq_pwm_data_collector(int step, void* config, void *data)
{
  return NULL;
}

void freq_pwm_ui_handler(void* data, void* config)
{
}

int freq_pwm_print_config(printf_func pfunc, void *device_config)
{
  return 0;
}

int freq_pwm_set_config(printf_func pfunc, int argc, char **argv, void *device_config)
{
  return 0;
}
