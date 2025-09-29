#include "board.h"
#include "multimeter.h"

enum multimeter_states {IDLE, CHECK};

static unsigned int capacitance_coefficient[2] = {CAPACITANCE_COEFFICIENT_1K, CAPACITANCE_COEFFICIENT_100K};
static unsigned int capacitance_offset[2] = {CAPACITANCE_OFFSET_1K, CAPACITANCE_OFFSET_100K};

static enum multimeter_modes multimeter_mode = FREQUENCY;

static enum multimeter_states multimeter_state = IDLE;

static unsigned int started_measurements = 0;

multimeter_result_t multimeter_result =
{
  .frequency_hz = 0,
  .inductance_nH = 0,
  .voltage_current = {{0, 0}, {0, 0}},
  .resistance_mOhm = {0, 0},
  .temperature_Cx100 = 0,
  .vdda_mV = 0,
  .capacitance = {0, 0, 0}
};

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

void multimeter_set_mode(enum multimeter_modes mode)
{
  multimeter_mode = mode;
}

static void start_measurements(void)
{
  start_voltage_measurements();
  started_measurements = VOLTAGE1_MEASUREMENT | VOLTAGE2_MEASUREMENT;
  switch (multimeter_mode)
  {
    case FREQUENCY:
    case INDUCTANCE:
      start_frequency_measurement();
      started_measurements |= FREQUENCY_MEASUREMENT;
      break;
    case CAPACITANCE:
      start_capacitance_measurement(CAPACITANCE_CHANNEL_1K);
      started_measurements |= CAPACITANCE_MEASUREMENT_1K;
      break;
    default: break;
  }
}

static unsigned int calculate_inductance(void)
{
  return 0;
}

static unsigned int check_extra(int channel, unsigned int extra_measurements)
{
  if (!extra_measurements)
    extra_measurements = start_extra_measurements(channel, 0);
  return extra_measurements;
}

static void finish_measurements(unsigned int changes)
{
  if (changes & VOLTAGE1_MEASUREMENT)
  {
    multimeter_result.voltage_current[0].voltage_uV = finish_voltage_measurement(0);
    start_current_measurement(0);
    started_measurements |= CURRENT1_MEASUREMENT;
  }
  if (changes & VOLTAGE2_MEASUREMENT)
  {
    multimeter_result.voltage_current[1].voltage_uV = finish_voltage_measurement(1);
    start_current_measurement(1);
    started_measurements |= CURRENT2_MEASUREMENT;
  }
  if (changes & CURRENT1_MEASUREMENT)
  {
    multimeter_result.voltage_current[0].current_nA = finish_current_measurement(0);
    if (multimeter_mode == RESISTANCE)
    {
      start_resistance_measurement(0, HIGH);
      started_measurements |= RESISTANCE1_MEASUREMENT_HIGH;
    }
    else
      started_measurements |= start_extra_measurements(0, 0);
  }
  if (changes & CURRENT2_MEASUREMENT)
  {
    multimeter_result.voltage_current[1].current_nA = finish_current_measurement(1);
    if (multimeter_mode == RESISTANCE)
    {
      start_resistance_measurement(1, HIGH);
      started_measurements |= RESISTANCE2_MEASUREMENT_HIGH;
    }
    else
      started_measurements |= start_extra_measurements(1, 0);
  }
  if (changes & TEMPERATURE_MEASUREMENT)
  {
    multimeter_result.temperature_Cx100 = finish_temperature_measurement();
    started_measurements |= start_extra_measurements(0, 1) | start_extra_measurements(1, 1);
  }
  if (changes & VDDA_MEASUREMENT)
  {
    multimeter_result.vdda_mV = finish_vdda_measurement();
    started_measurements |= start_extra_measurements(0, 2) | start_extra_measurements(1, 2);
  }
  if (changes & FREQUENCY_MEASUREMENT)
  {
    multimeter_result.frequency_hz = finish_frequency_measurement();
    if (multimeter_mode == INDUCTANCE)
      multimeter_result.inductance_nH = calculate_inductance();
  }
  if (changes & RESISTANCE1_MEASUREMENT_HIGH)
    started_measurements |= check_extra(0, finish_resistance_measurement(0, HIGH));
  if (changes & RESISTANCE2_MEASUREMENT_HIGH)
    started_measurements |= check_extra(1, finish_resistance_measurement(1, HIGH));
  if (changes & RESISTANCE1_MEASUREMENT_MEDIUM)
    started_measurements |= check_extra(0, finish_resistance_measurement(0, MEDIUM));
  if (changes & RESISTANCE2_MEASUREMENT_MEDIUM)
    started_measurements |= check_extra(1, finish_resistance_measurement(1, MEDIUM));
  if (changes & RESISTANCE1_MEASUREMENT_LOW)
    started_measurements |= check_extra(0, finish_resistance_measurement(0, LOW));
  if (changes & RESISTANCE2_MEASUREMENT_LOW)
    started_measurements |= check_extra(1, finish_resistance_measurement(1, LOW));
  if (changes & CAPACITANCE_MEASUREMENT_1K)
    started_measurements |= finish_capacitance_measurement_1k();
  if (changes & CAPACITANCE_MEASUREMENT_100K)
    finish_capacitance_measurement();
}

unsigned int multimeter_timer_event(void)
{
  unsigned int changes = 0;
  switch (multimeter_state)
  {
    case IDLE:
      multimeter_state = CHECK;
      start_measurements();
      break;
    case CHECK:
      changes = check_measurements_statuses();
      if (changes)
      {
        finish_measurements(changes);
        started_measurements &= ~changes;
        if (!started_measurements)
          multimeter_state = IDLE;
      }
      break;
  }
  return changes;
}
