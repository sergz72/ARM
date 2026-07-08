#include "board.h"
#include "multimeter.h"
#include <limits.h>
#include <string.h>
#include <systick.h>

const unsigned int dac_levels[MAX_LEVEL+1] = {DAC_LEVEL_0, DAC_LEVEL_1, DAC_LEVEL_2, DAC_LEVEL_3, DAC_LEVEL_4};
const int adc_gains[MAX_LEVEL+1] = {ADC_GAIN_LEVEL_0, ADC_GAIN_LEVEL_1, ADC_GAIN_LEVEL_2, ADC_GAIN_LEVEL_3, ADC_GAIN_LEVEL_4};

const calibration_data_t *calibration_data = (const calibration_data_t*)EEPROM_START;
static calibration_data_t calibration_data_buffer;

int calibrateR(int level, int R, int *result)
{
  if (level < 0 || level > MAX_LEVEL)
    return 1;
  dac_set(dac_levels[level]);
  delayms(1000);
  int uV;
  int rc = adc_read(adc_gains[level], &uV);
  if (rc)
    return rc;
  if (uV == INT_MAX)
    return 1;
  if (uV < 0)
    return 2;
  *result = uV;

  memccpy(&calibration_data_buffer, calibration_data, 0, sizeof(calibration_data_t));

  calibration_data_buffer.vr[level].r = R;
  calibration_data_buffer.vr[level].v = uV;

  return eeprom_write(&calibration_data_buffer, sizeof(calibration_data_t));
}

int calculateR(getr_result *value, printf_func pfunc)
{
  for (value->level = 0; value->level <= MAX_LEVEL; value->level++)
  {
    dac_set(dac_levels[value->level]);
    delayms(1000);
    int rc = adc_read(adc_gains[value->level], &value->uV);
    if (rc)
      return rc;
    if (pfunc)
      pfunc("adc_read[%d] = %d uV\r\n", value->level, value->uV);
    if (value->uV == INT_MAX)
    {
      value->r = LONG_LONG_MAX;
      return 0;
    }
    if (value->uV < 0)
      value->uV = 0;
    if ((value->uV >= ADC_REFERENCE_VOLTAGE / 20) || (value->level == MAX_LEVEL))
    {
      value->r = (long long int)calibration_data->vr[value->level].r * (long long int)value->uV / calibration_data->vr[value->level].v;
      return 0;
    }
  }

  return 100;
}
