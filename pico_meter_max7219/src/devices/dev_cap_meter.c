//#include <stdio.h>
#include "board.h"
#include "dev_cap_meter.h"
#include "hardware/pio.h"

void* cap_meter_initializer(void)
{
  return NULL;
}

void *cap_meter_data_collector(int step, void* config, void *data)
{
  return NULL;
}

void cap_meter_ui_handler(void* data, void* config)
{

}

int cap_meter_print_config(printf_func pfunc, void *device_config)
{
  return 0;
}

int cap_meter_set_config(printf_func pfunc, int argc, char **argv, void *device_config)
{
  return 0;
}

void cap_meter_init(void)
{
  if (cap_value_updated)
  {
    //puts("cap_value_updated");
    cap_value_updated = 0;
    gpio_set_dir(CAP_RLOW_PIN, false);
    pio_sm_put_blocking(pio0, 2, CAP_MAX);
  }
}

unsigned int calculate_capacitance(void)
{
  return cap_value * 23 / 10;
}