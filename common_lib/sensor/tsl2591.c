#include "board.h"
#include <tsl2591.h>

#define REG_ENABLE  0xA0
#define REG_CONFIG  0xA1
#define REG_AILTL   0xA4
#define REG_AILTH   0xA5
#define REG_AIHTL   0xA6
#define REG_AIHTH   0xA7
#define REG_NPAILTL 0xA8
#define REG_NPAILTH 0xA9
#define REG_NPAIHTL 0xAA
#define REG_NPAIHTH 0xAB
#define REG_PERSIST 0xAC
#define REG_PID     0xB1
#define REG_ID      0xB2
#define REG_STATUS  0xB3
#define REG_C0DATAL 0xB4
#define REG_C0DATAH 0xB5
#define REG_C1DATAL 0xB6
#define REG_C1DATAH 0xB7

#define TSL2591_LUX_DF    408.0f   ///< Lux cooefficient

static const unsigned int gain_scaling[4] = {1, 25, 428, 9876};
static const tsl2591_config *config;

static unsigned char itime;

static int tsl2591_set_gain(unsigned char gain)
{
  return tsl2591_write(REG_CONFIG, (gain << 4) | itime);
}

int tsl2591_set_als_thresholds(const tsl2591_thresholds *thresholds)
{
  int rc = tsl2591_write(REG_AILTL, (unsigned char)thresholds->low);
  if (rc)
    return rc;
  rc = tsl2591_write(REG_AILTH, (unsigned char)(thresholds->low >> 8));
  if (rc)
    return rc;
  rc = tsl2591_write(REG_AIHTL, (unsigned char)thresholds->high);
  if (rc)
    return rc;
  return tsl2591_write(REG_AIHTH, (unsigned char)(thresholds->high >> 8));
}

int tsl2591_set_no_persist_als_thresholds(const tsl2591_thresholds *thresholds)
{
  int rc = tsl2591_write(REG_NPAILTL, (unsigned char)thresholds->low);
  if (rc)
    return rc;
  rc = tsl2591_write(REG_NPAILTH, (unsigned char)(thresholds->low >> 8));
  if (rc)
    return rc;
  rc = tsl2591_write(REG_NPAIHTL, (unsigned char)thresholds->high);
  if (rc)
    return rc;
  return tsl2591_write(REG_NPAIHTH, (unsigned char)(thresholds->high >> 8));
}

int tsl2591_set_persistence_filter(unsigned char filter)
{
  return tsl2591_write(REG_PERSIST, filter);
}

int tsl2591_enable(unsigned char enable)
{
  return tsl2591_write(REG_ENABLE, enable);
}

int tsl2591_init(const tsl2591_config *conf)
{
  unsigned char value;

  config = conf;

  if (conf->integration_time_ms != 100 && conf->integration_time_ms != 200 && conf->integration_time_ms != 300 &&
      conf->integration_time_ms != 400 && conf->integration_time_ms != 500 && conf->integration_time_ms != 600)
    return 1;

  itime = (unsigned char)((config->integration_time_ms - 100) / 100);

  int rc = tsl2591_read8(REG_ID, &value);
  if (rc)
    return rc;
  if (value != 0x50)
    return value | 0x100;
  rc = tsl2591_read8(REG_PID, &value);
  if (rc)
    return rc;
  if ((value & 0x30) != 0)
    return value | 0x200;

  value = 3;
  if (config->als_interrupt_enable)
    value |= 0x10;
  if (config->sleep_after_interrupt)
    value |= 0x40;
  if (config->no_persist_interrupt_enable)
    value |= 0x80;
  rc = tsl2591_enable(value);
  if (rc)
    return rc;

  rc = tsl2591_set_gain(0);
  if (rc)
    return rc;

  rc = tsl2591_set_als_thresholds(&config->als_thresholds);
  if (rc)
    return rc;

  rc = tsl2591_set_no_persist_als_thresholds(&config->no_persist_als_thresholds);
  if (rc)
    return rc;

  return tsl2591_set_persistence_filter(config->persistence_filter);
}

int tsl2591_clear_interrupts(void)
{
  return tsl2591_command(0xE7);
}

int tsl2591_get_status(unsigned char *status)
{
  return tsl2591_read8(REG_STATUS, status);
}

int tsl2591_measure(tsl2591_result *result)
{
  unsigned short ch0, ch1;
  float ch0f, ch1f;
  int rc;

  result->gain = 0;
  result->tries = 1;

  for (;;)
  {
    rc = tsl2591_read16(REG_C0DATAL, &ch0);
    if (rc)
      return rc;
    rc = tsl2591_read16(REG_C1DATAL, &ch1);
    if (rc)
      return rc;
    if ((ch0 == 0xFFFF || ch1 == 0xFFFF) && result->gain != 0)
    {
      rc = tsl2591_set_gain(--result->gain);
      if (rc)
        return rc;
      delayms(2000);
      result->tries++;
      continue;
    }
    if (result->gain != 0 || (ch0 > 65535/30 || ch1 > 65535/30))
      break;
    if (ch0 > 65535/450 || ch1 > 65535/450)
      result->gain = 1;
    else if (ch0 > 65535/10000 || ch1 > 65535/10000)
      result->gain = 2;
    else
      result->gain = 3;
    rc = tsl2591_set_gain(result->gain);
    if (rc)
      return rc;
    result->tries++;
    delayms(2000);
  }
  if (result->gain != 0)
  {
    rc = tsl2591_set_gain(0);
    if (rc)
      return rc;
  }

  ch0f = (float)ch0;
  ch1f = (float)ch1;
  float cpl = (float)(config->integration_time_ms * gain_scaling[result->gain]) / TSL2591_LUX_DF;
  // Alternate lux calculation 1
  // See: https://github.com/adafruit/Adafruit_TSL2591_Library/issues/14
  result->lux = (ch0f - ch1f) * (1.0f - ch1f / ch0f) / cpl;
  return 0;
}
