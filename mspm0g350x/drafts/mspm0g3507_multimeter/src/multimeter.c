#include "board.h"
#include "multimeter.h"
#include <limits.h>
#include <string.h>

static const unsigned int dac_levels[DAC_MAX_LEVEL+1] = {DAC_LEVEL_0, DAC_LEVEL_1, DAC_LEVEL_2, DAC_LEVEL_3};

const calibration_data_t *calibration_data = (const calibration_data_t*)EEPROM_START;
static calibration_data_t calibration_data_buffer;

int calibrateR(int level, int gain, int R, int *result)
{
  dac_set(dac_levels[level]);
  int uV;
  int rc = adc_read(gain, &uV);
  if (rc)
    return rc;
  if (uV == INT_MAX)
    return 1;
  if (uV < 0)
    return 2;
  *result = uV;

  memccpy(&calibration_data_buffer, calibration_data, 0, sizeof(calibration_data_t));

  calibration_data_buffer.r[level][gain] = R;
  calibration_data_buffer.v[level][gain] = *result;

  return eeprom_write(&calibration_data_buffer, sizeof(calibration_data_t));
}

int calculateR(unsigned long long int *value)
{
  int gain = 0;
  for (int level = 0; level <= DAC_MAX_LEVEL; level++)
  {
    dac_set(dac_levels[level]);
    int uV;
    int rc = adc_read(gain, &uV);
    if (rc)
      return rc;
    if (uV == INT_MAX)
    {
      *value = LONG_LONG_MAX;
      return 0;
    }
    if (uV < 0)
      uV = 0;
    if (uV >= ADC_REFERENCE_VOLTAGE / 20 || level == DAC_MAX_LEVEL)
    {
      while (uV < ADC_REFERENCE_VOLTAGE / 20 && gain <= ADC_MAX_GAIN)
      {
        gain++;
        rc = adc_read(gain, &uV);
        if (rc)
          return rc;
      }
      if (uV < 0)
        uV = 0;
      *value = calibration_data->r[level][gain] * (long long int)uV / calibration_data->v[level][gain];
      return 0;
    }
  }

  return 100;
}
