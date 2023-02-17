#include "board.h"
#include <dev_freq_pwm.h>
#include "ui.h"
#include <stdio.h>

void *freq_pwm_initializer(void)
{
  return (void*)1;
}

void freq_pwm_ui_init_handler(void* config)
{
  printf("freq_pwm_ui_init_handler\n");
  LED_Write_String(0, "F       ");
  LED_Write_String(1, "        ");
  LED_Write_String(2, "   1   1");
  LED_Write_String(3, "F1D1F2D2");
}

void freq_pwm_ui_handler(void* data, void* config)
{

}

int freq_pwm_ui_keyboard_handler(void *config, unsigned int event)
{
  return 0;
}

int freq_pwm_print_config(printf_func pfunc, void *device_config)
{
  return 0;
}

int freq_pwm_set_config(printf_func pfunc, int argc, char **argv, void *device_config)
{
  return 0;
}
