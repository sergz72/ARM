//#include <stdio.h>
#include "board.h"
#include "cap_meter.h"
#include "hardware/pio.h"

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