#include "board.h"
#include <ina228.h>

static bool adcRange[INA228_MAX_CHANNELS];

int ina228SetConfig(int channel, unsigned char address, const INA228Config cfg)
{
  adcRange[channel] = cfg.bits.adcrange != 0;
  return ina228WriteRegister16(channel, address, INA228_REG_CONFIG, cfg.raw);
}

int ina228SetAdcConfig(int channel, unsigned char address, const INA228AdcConfig cfg)
{
  return ina228WriteRegister16(channel, address, INA228_REG_ADC_CONFIG, cfg.raw);
}

int ina228GetBusVoltage(int channel, unsigned char address, int *voltage)
{
  int v;
  int rc = ina228ReadRegister24(channel, address, INA228_REG_VBUS, (unsigned int*)&v);
  if (!rc)
  {
    v >>= 4;
    if (v & 0x80000)
      v |= 0xFFF00000;
    *voltage = (int)(((long long int)v * 204800000L) >> 20);
  }
  return rc;
}

int ina228GetShuntCurrent(int channel, unsigned char address, int R, int *current)
{
  int i;
  int rc = ina228ReadRegister24(channel, address, INA228_REG_VSHUNT, (unsigned int*)&i);
  if (!rc)
  {
    i >>= 4;
    if (i & 0x80000)
      i |= 0xFFF00000;
    *current = (int)((long long int)i * (adcRange[channel] ? 78125LL : 312500LL) / R);
  }
  return rc;
}

int ina228GetTemperature(int channel, unsigned char address, int *temperature)
{
  short t;
  int rc = ina228ReadRegister16(channel, address, INA228_REG_DIETEMP, (unsigned short*)&t);
  if (!rc)
    *temperature = (512000 * (int)t) >> 16;
  return rc;
}
