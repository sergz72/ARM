#include "board.h"
#include "multimeter.h"

static unsigned int capacitance_coefficient[2] = {CAPACITANCE_COEFFICIENT_1K, CAPACITANCE_COEFFICIENT_100K};
static unsigned int capacitance_offset[2] = {CAPACITANCE_OFFSET_1K, CAPACITANCE_OFFSET_100K};

static void calculate_capacitance(int channel, unsigned int difference, capacitance_result *result)
{
  result->diff = difference;
  unsigned int pf = (unsigned int)((unsigned long long int)difference * 100000UL / capacitance_coefficient[channel]);
  if (pf <= capacitance_offset[channel])
    result->pF = 0;
  else
    result->pF = pf - capacitance_offset[channel];
  result->channel = channel;
}

int get_capacitance(capacitance_result *result)
{
  capacity_measurement_start(CAPACITANCE_CHANNEL_1K);
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
  charge_off();
  discharge_on();
  if (!timeout)
    return 1;
  unsigned int difference = end_time - start_time;
  if (difference >= CAPACITANCE_MIN_VALUE_1K)
  {
    calculate_capacitance(CAPACITANCE_CHANNEL_1K, difference, result);
    return 0;
  }

  delayms(500);
  capacity_measurement_start(CAPACITANCE_CHANNEL_100K);
  timeout = 3000;
  while (timeout)
  {
    delayms(1);
    if (capacity_measurement_done)
      break;
    timeout--;
  }
  capacity_measurement_done = 0;
  start_time = get_capacity_measurement_start_time();
  end_time = get_capacity_measurement_end_time();
  charge_off();
  discharge_on();
  if (!timeout)
    return 1;
  difference = end_time - start_time;
  calculate_capacitance(CAPACITANCE_CHANNEL_100K, difference, result);

  return 0;
}
