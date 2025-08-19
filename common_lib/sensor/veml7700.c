#include "board.h"
#include <veml7700.h>

#define REG_CONFIG     0
#define REG_POWER_SAVE 3
#define REG_ALS        4
#define REG_ID         7

static int veml7700_set_config(unsigned int gainx8)
{
  unsigned short config = 3 << 6; // integration time is 800 ms

  switch (gainx8)
  {
    case 1: //gain is 1/8
      config |= 2 << 11;
      break;
    case 2: //gain is 1/4
      config |= 3 << 11;
      break;
    case 16: //gain is 2
      config |= 1 << 11;
      break;
    default:
      break;
  }
  return veml7700_write(REG_CONFIG, config);
}

int veml7700_init(void)
{
  unsigned short value;

  int rc = veml7700_read(REG_ID, &value);
  if (rc)
    return rc;
  if (value != 0xC481)
    return value == 0 ? -1 : value;
  rc = veml7700_set_config(1);
  if (rc)
    return rc;
  return veml7700_write(REG_POWER_SAVE, 0); // disable power save
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

int veml7700_measure(veml7700_result *result)
{
  unsigned short value;

  result->gainx8 = 1;
  result->tries = 1;
  for (;;)
  {
    int rc = veml7700_read(REG_ALS, &value);
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
    rc = veml7700_read(REG_ALS, &value);
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
