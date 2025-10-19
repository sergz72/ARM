#include "board.h"
#include "multimeter.h"
#include <limits.h>

static unsigned int capacitance_coefficient[3] = {0, CAPACITANCE_COEFFICIENT_1K, CAPACITANCE_COEFFICIENT_100K};
static unsigned int capacitance_offset[3] = {0, CAPACITANCE_OFFSET_1K, CAPACITANCE_OFFSET_100K};

enum multimeter_modes multimeter_mode = FREQUENCY;

static unsigned int started_measurements = 0;

multimeter_result_t multimeter_result =
{
  .frequency_hz = 0,
  .inductance_nH = 0,
  .diode_voltage_uV = {0,0},
  .voltage_current = {{0, 0}, {0, 0}},
  .resistance_mOhm = {0, 0},
  .temperature_Cx10 = 0,
  .vdda_mV = 0,
  .capacitance = {0, 0, 0}
};

int planned_capacitance_measurement = 0;

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

static void start_capacitance_measurement(int channel)
{
  planned_capacitance_measurement = channel;
}

static unsigned int check_capacitance_measurement_status(void)
{
  if (planned_capacitance_measurement && capacitor_is_discharged())
  {
    capacitance_measurement_start(planned_capacitance_measurement);
    planned_capacitance_measurement = 0;
    return 0;
  }
  if (capacitance_measurement_done)
  {
    charge_off();
    discharge_on();
    capacitance_measurement_done = 0;
    return started_measurements & (CAPACITANCE_MEASUREMENT_100K | CAPACITANCE_MEASUREMENT_1K);
  }
  return 0;
}

static void finish_capacitance_measurement(void)
{
  unsigned int start_time = get_capacitance_measurement_start_time();
  unsigned int end_time = get_capacitance_measurement_end_time();
  unsigned int difference = end_time - start_time;
  calculate_capacitance(CAPACITANCE_CHANNEL_100K, difference, &multimeter_result.capacitance);
}

static unsigned int finish_capacitance_measurement_1k(void)
{
  unsigned int start_time = get_capacitance_measurement_start_time();
  unsigned int end_time = get_capacitance_measurement_end_time();
  unsigned int difference = end_time - start_time;
  if (difference >= CAPACITANCE_MIN_VALUE_1K)
  {
    calculate_capacitance(CAPACITANCE_CHANNEL_1K, difference, &multimeter_result.capacitance);
    return 0;
  }

  start_capacitance_measurement(CAPACITANCE_CHANNEL_100K);
  return CAPACITANCE_MEASUREMENT_100K;
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

/*
 * L = (NOM / F / F / C) * 100 / PI2
 * C in pF
 * L in uH
 * F in HZ
 */
#define NOM (2500000000 * 100000000)
#define PI2 987
#define DEFAULT_L_CAP 3400 //pF
static unsigned int calculate_inductance(void)
{
  unsigned long long int l;
  unsigned long long int f2;
  unsigned long long int c = DEFAULT_L_CAP / 100;
  if (!c || !multimeter_result.frequency_hz)
    return UINT_MAX;
  f2 = (unsigned long long int)multimeter_result.frequency_hz * multimeter_result.frequency_hz;
  l = NOM / f2 / c;
  return l * 100 / PI2;
}

static unsigned int check_extra(int channel, unsigned int extra_measurements)
{
  if (!extra_measurements)
    extra_measurements = start_extra_measurements(channel, 0);
  return extra_measurements;
}

static unsigned int build_changes(unsigned int *value, unsigned int new_value, unsigned int change_flag)
{
  if (new_value != *value)
  {
    *value = new_value;
    return change_flag;
  }
  return 0;
}

static unsigned int finish_measurements(unsigned int finished_measurements)
{
  unsigned int changes = 0;
  if (finished_measurements & VOLTAGE1_MEASUREMENT)
  {
    changes |= build_changes((unsigned int*)&multimeter_result.voltage_current[0].voltage_uV, finish_voltage_measurement(0), VOLTAGE1_CHANGED);
    start_current_measurement(0);
    started_measurements |= CURRENT1_MEASUREMENT;
  }
  if (finished_measurements & VOLTAGE2_MEASUREMENT)
  {
    changes |= build_changes((unsigned int*)&multimeter_result.voltage_current[1].voltage_uV, finish_voltage_measurement(1), VOLTAGE2_CHANGED);
    start_current_measurement(1);
    started_measurements |= CURRENT2_MEASUREMENT;
  }
  if (finished_measurements & CURRENT1_MEASUREMENT)
  {
    changes |= build_changes((unsigned int*)&multimeter_result.voltage_current[0].current_nA, finish_current_measurement(0), CURRENT1_CHANGED);
    if (multimeter_mode == RESISTANCE)
    {
      start_resistance_measurement(0, HIGH);
      started_measurements |= RESISTANCE1_MEASUREMENT_HIGH;
    }
    else if (multimeter_mode == DIODE_TEST)
      start_diode_voltage_measurement(0);
    else
      started_measurements |= start_extra_measurements(0, 0);
  }
  if (finished_measurements & CURRENT2_MEASUREMENT)
  {
    changes |= build_changes((unsigned int*)&multimeter_result.voltage_current[1].current_nA, finish_current_measurement(1), CURRENT2_CHANGED);
    if (multimeter_mode == RESISTANCE)
    {
      start_resistance_measurement(1, HIGH);
      started_measurements |= RESISTANCE2_MEASUREMENT_HIGH;
    }
    else if (multimeter_mode == DIODE_TEST)
      start_diode_voltage_measurement(1);
    else
      started_measurements |= start_extra_measurements(1, 0);
  }
  if (finished_measurements & DIODE_VOLTAGE_MEASUREMENT1)
  {
    changes |= build_changes(&multimeter_result.diode_voltage_uV[0], finish_diode_voltage_measurement(0), DIODE_VOLTAGE1_CHANGED);
    started_measurements |= start_extra_measurements(0, 0);
  }
  if (finished_measurements & DIODE_VOLTAGE_MEASUREMENT2)
  {
    changes |= build_changes(&multimeter_result.diode_voltage_uV[1], finish_diode_voltage_measurement(1), DIODE_VOLTAGE2_CHANGED);
    started_measurements |= start_extra_measurements(1, 0);
  }
  if (finished_measurements & TEMPERATURE_MEASUREMENT)
  {
    changes |= build_changes(&multimeter_result.temperature_Cx10, finish_temperature_measurement(), TEMPERATURE_CHANGED);
    started_measurements |= start_extra_measurements(0, 1) | start_extra_measurements(1, 1);
  }
  if (finished_measurements & VDDA_MEASUREMENT)
  {
    changes |= build_changes(&multimeter_result.vdda_mV, finish_vdda_measurement(), VDDA_CHANGED);
    started_measurements |= start_extra_measurements(0, 2) | start_extra_measurements(1, 2);
  }
  if (finished_measurements & FREQUENCY_MEASUREMENT)
  {
    changes |= build_changes(&multimeter_result.frequency_hz, finish_frequency_measurement(), FREQUENCY_CHANGED);
    if (multimeter_mode == INDUCTANCE)
      changes |= build_changes(&multimeter_result.inductance_nH, calculate_inductance(), INDUCTANCE_CHANGED);
  }
  unsigned int prev_resistance[2];
  prev_resistance[0] = multimeter_result.resistance_mOhm[0];
  prev_resistance[1] = multimeter_result.resistance_mOhm[1];
  if (finished_measurements & RESISTANCE1_MEASUREMENT_HIGH)
    started_measurements |= check_extra(0, finish_resistance_measurement(0, HIGH));
  if (finished_measurements & RESISTANCE2_MEASUREMENT_HIGH)
    started_measurements |= check_extra(1, finish_resistance_measurement(1, HIGH));
  if (finished_measurements & RESISTANCE1_MEASUREMENT_MEDIUM)
    started_measurements |= check_extra(0, finish_resistance_measurement(0, MEDIUM));
  if (finished_measurements & RESISTANCE2_MEASUREMENT_MEDIUM)
    started_measurements |= check_extra(1, finish_resistance_measurement(1, MEDIUM));
  if (finished_measurements & RESISTANCE1_MEASUREMENT_LOW)
    started_measurements |= check_extra(0, finish_resistance_measurement(0, LOW));
  if (finished_measurements & RESISTANCE2_MEASUREMENT_LOW)
    started_measurements |= check_extra(1, finish_resistance_measurement(1, LOW));
  if (prev_resistance[0] != multimeter_result.resistance_mOhm[0])
    changes |= RESISTANCE1_CHANGED;
  if (prev_resistance[1] != multimeter_result.resistance_mOhm[1])
    changes |= RESISTANCE2_CHANGED;
  unsigned int prev_capacitance = multimeter_result.capacitance.pF;
  if (finished_measurements & CAPACITANCE_MEASUREMENT_1K)
    started_measurements |= finish_capacitance_measurement_1k();
  if (finished_measurements & CAPACITANCE_MEASUREMENT_100K)
    finish_capacitance_measurement();
  if (prev_capacitance != multimeter_result.capacitance.pF)
    changes |= CAPACITANCE_CHANGED;
  return changes;
}

unsigned int multimeter_timer_event(void)
{
  unsigned int changes = 0;
  if (!started_measurements)
    start_measurements();
  else
  {
    unsigned int finished_measurements = check_measurements_statuses() | check_capacitance_measurement_status();
    if (finished_measurements)
    {
      changes = finish_measurements(finished_measurements);
      started_measurements &= ~finished_measurements;
    }
  }
  return changes;
}
