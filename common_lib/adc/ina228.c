#include "board.h"
#include <ina228.h>

static bool adcRange[INA228_MAX_CHANNELS];

int ina228SetConfig(int channel, unsigned char address, const INA228Config cfg)
{
  adcRange[channel] = cfg.adcrange != 0;
  return ina228WriteRegister(channel, address, INA228_REG_CONFIG, &cfg, sizeof(cfg));
}

int ina228SetAdcConfig(int channel, unsigned char address, const INA228AdcConfig cfg)
{
  return ina228WriteRegister(channel, address, INA228_REG_ADC_CONFIG, &cfg, sizeof(cfg));
}

int ina228GetBusVoltage(int channel, unsigned char address, int *voltage)
{
  int v;
  int rc = ina228ReadRegister(channel, address, INA228_REG_VBUS, &v, 3);
  if (!rc)
  {
    v >>= 4;
    if (v & 0x80000)
      v |= 0xFFF00000;
    *voltage = (int)(((long long int)v * 102400000L) >> 20);
  }
  return rc;
}

int ina228GetShuntCurrent(int channel, unsigned char address, int R, int *current)
{
  int i;
  int rc = ina228ReadRegister(channel, address, INA228_REG_VSHUNT, &i, 3);
  if (!rc)
  {
    i >>= 4;
    if (i & 0x80000)
      i |= 0xFFF00000;
    *current = (int)(((long long int)i * (adcRange[channel] ? 40960000000L : 163840000000L)) >> 20) / R;
  }
  return rc;
}

int ina228GetTemperature(int channel, unsigned char address, int *temperature)
{
  int t;
  int rc = ina228ReadRegister(channel, address, INA228_REG_VSHUNT, &t, 2);
  if (!rc)
  {
    if (t & 0x8000)
      t |= 0xFFFF0000;
    *temperature = (256000 * t) >> 16;
  }
  return rc;
}
