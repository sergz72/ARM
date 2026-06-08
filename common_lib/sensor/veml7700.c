#include "board.h"
#include <veml7700.h>

static int veml7700_set_config(unsigned int gainx8)
{
  unsigned short config = VEML7700_IT_800ms; // integration time is 800 ms

  switch (gainx8)
  {
    case 1: //gain is 1/8
      config |= VEML7700_GAIN_1d8;
      break;
    case 2: //gain is 1/4
      config |= VEML7700_GAIN_1d4;
      break;
    case 16: //gain is 2
      config |= VEML7700_GAIN_2;
      break;
    default:
      break;
  }

  return veml7700_write(VEML7700_REG_CONFIG, config);
}

static int veml7700_check_id(void)
{
  unsigned short value;

  int rc = veml7700_read(VEML7700_REG_ID, &value);
  if (rc)
    return rc;
  if (value != 0xC481)
    return value == 0 ? -1 : value;
  return 0;
}

int veml7700_init(void)
{
  int rc = veml7700_check_id();
  if (rc)
    return rc;
  rc = veml7700_set_config(1);
  if (rc)
    return rc;
  return veml7700_write(VEML7700_REG_POWER_SAVE, 0); // disable power save
}

int veml7700_ex_init(const unsigned short config, const unsigned short power_save)
{
  int rc = veml7700_check_id();
  if (rc)
    return rc;
  rc = veml7700_write(VEML7700_REG_CONFIG, config);
  if (rc)
    return rc;
  return veml7700_write(VEML7700_REG_POWER_SAVE, power_save);
}

static float calculate_lux(unsigned short value, unsigned int gainx8)
{
  switch (gainx8)
  {
    case 1: //gain is 1/8
      return (float)value * 0.0672f;
    case 2: //gain is 1/4
      return (float)value * 0.0366f;
    case 8: //gain is 1
      return (float)value * 0.0084f;
    default: // gain is 2
      return (float)value * 0.0042f;
  }
}

int veml7700_measure(veml7700_result *result, int wait)
{
  unsigned short value;

  result->gainx8 = 1;
  result->tries = 1;
  for (;;)
  {
    if (wait)
      delayms(1500);
    int rc = veml7700_read(VEML7700_REG_ALS, &value);
    if (rc)
      return rc;
    if (value >= 0x7000)
    {
      result->lux = calculate_lux(value, 1);
      return 0;
    }
    result->tries++;
    if (value >= 0x1000)
      result->gainx8 = 2;
    else if (value >= 0x500)
      result->gainx8 = 8;
    else
      result->gainx8 = 16;
    rc = veml7700_set_config(result->gainx8);
    if (rc)
      return rc;
    delayms(1500);
    rc = veml7700_read(VEML7700_REG_ALS, &value);
    if (rc)
      return rc;
    rc = veml7700_set_config(1);
    if (rc)
      return rc;
    if (value == 0xFFFF) // overflow
    {
      result->gainx8 = 1;
      result->tries++;
      delayms(1500);
    }
    else
    {
      result->lux = calculate_lux(value, result->gainx8);
      break;
    }
  }
  return 0;
}
