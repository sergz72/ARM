#include <board.h>
#include <pir_sensor.h>

volatile unsigned short pir_filter_output;
static unsigned short pir_adc_samples[PIR_SENSOR_AVERAGING_FILTER_SAMPLES_COUNT];
static unsigned int pir_adc_samples_sum;
volatile unsigned short pir_adc_samples_avg;
static unsigned int pir_adc_samples_idx;

void pir_sensor_init(void)
{
  pir_filter_output = 0;
}

void pir_sensor_adc_handler(unsigned short value)
{
  if (value > pir_filter_output)
    pir_filter_output = pir_filter_output + ((value - pir_filter_output) >> PIR_SENSOR_FILTER_CRS);
  else
    pir_filter_output = pir_filter_output - ((pir_filter_output - value) >> PIR_SENSOR_FILTER_CRS);

  pir_adc_samples_sum -= pir_adc_samples[pir_adc_samples_idx];
  pir_adc_samples[pir_adc_samples_idx] = pir_filter_output;
  if (pir_adc_samples_idx >= PIR_SENSOR_AVERAGING_FILTER_SAMPLES_COUNT - 1)
    pir_adc_samples_idx = 0;
  else
    pir_adc_samples_idx++;
  pir_adc_samples_sum += pir_filter_output;
  pir_adc_samples_avg = (unsigned short)(pir_adc_samples_sum / PIR_SENSOR_AVERAGING_FILTER_SAMPLES_COUNT);

  if (pir_filter_output > pir_adc_samples_avg + PIR_SENSOR_FILTER_THRESHOLD ||
    pir_filter_output < pir_adc_samples_avg - PIR_SENSOR_FILTER_THRESHOLD)
    pir_motion_detected();
}