#include "board.h"
#include "multimeter.h"
#include <delay_systick.h>

#define CAPACITANCE_COEFFICIENT 1962851UL

static unsigned int capacitance_offset;

void multimeter_init(void)
{
  capacitance_offset = 456 * 100000 / CAPACITANCE_COEFFICIENT;
}

int get_capacitance(unsigned int *pF, unsigned int *diff)
{
  capacity_measurement_start();
  int timeout = 3000;
  while (timeout)
  {
    delayms(1);
    if (capacity_measurement_done)
      break;
    timeout--;
  }
  capacity_measurement_done = 0;
  unsigned int start_time = get_capacity_measurement_start_time();
  unsigned int end_time = get_capacity_measurement_end_time();
  CHARGE_OFF;
  discharge_on();
  if (!timeout)
    return 1;
  unsigned int difference = end_time - start_time;
  *diff = difference;
  unsigned int pf = (unsigned int)((unsigned long long int)difference * 100000UL / CAPACITANCE_COEFFICIENT);
  if (pf <= capacitance_offset)
    *pF = 0;
  else
    *pF = pf - capacitance_offset;
  return 0;
}
