#include "board.h"
#include "multimeter.h"
#include <ad7793.h>
#include <ads1220.h>

#define CAPACITANCE_COEFFICIENT_100K 1962851UL
#define CAPACITANCE_COEFFICIENT_1K 19628UL

#define MIN_VALUE_1K 200000

static const unsigned int capacitance_coefficient[2] = {CAPACITANCE_COEFFICIENT_1K, CAPACITANCE_CHANNEL_100K};

static const ad7793_configuration ad7793_config =
{
  .channel = AD7793_CHANNEL_AIN1N_AIN1N,
  .boost = 1,
  .buffer_enable = 1,
  .burnout_enable = 0,
  .gain = AD7793_GAIN_1,
  .internal_reference = 1,
  .unipolar = 1,
  .vbias = AD7793_VBIAS_AIN2M
};

static const ads1220_configuration ads1220_config =
{
  .burnout_sources_enable = 0,
  .continuous_conversion_mode = 0,
  .data_rate = ADS1220_DATA_RATE_20,
  .temperature_sensor_mode = 0,
  .mode = ADS1220_MODE_NORMAL,
  .pga_bypass = 0,
  .auto_close_low_side_switch = 0,
  .data_ready_on_dout = 0,
  .filter_50_60 = ADS1220_FILTER_50,
  .gain = ADS1220_PGA_GAIN_1,
  .idac1_mux = ADS1220_IDAC_MUX_REFN0,
  .idac2_mux = ADS1220_IDAC_MUX_OFF,
  .idac_current = ADS1220_IDAC_10,
  .mux = ADS1220_MUX_AIN0_AIN1,
  .vref = ADS1220_VREF_INTERNAL
};

static unsigned int capacitance_offset[2];

int multimeter_init(void)
{
  capacitance_offset[CAPACITANCE_CHANNEL_1K] = 456 * 100000 / CAPACITANCE_COEFFICIENT_1K;
  capacitance_offset[CAPACITANCE_CHANNEL_100K] = 456 * 100000 / CAPACITANCE_COEFFICIENT_100K;

  ad7793_init();
  ads1220_init();

  ad7793_set_mode_and_update_rate(AD7793_CHANNEL, AD7793_MODE_IDLE, AD7793_UPDATE_RATE_16_80);
  ad7793_set_configuration(AD7793_CHANNEL, &ad7793_config);
  ad7793_set_io(AD7793_CHANNEL, AD7793_IEXCDIR_IEXC1_IOUT1_IEXC2_IOUT2, AD7793_IEXCEN_10);

  ads1220_set_configuration(ADS1220_CHANNEL, &ads1220_config);

  return 0;
}

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
  if (difference >= MIN_VALUE_1K)
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
