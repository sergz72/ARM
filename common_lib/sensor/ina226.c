#include <ina226.h>

int inaReset(int channel, unsigned char address)
{
  return inaWriteRegister(channel, address, INA_REG_CONFIG, 0x8000);
}

int ina226SetConfig(int channel, unsigned char address, const INA226Config *cfg)
{
  return inaWriteRegister(channel, address, INA_REG_CONFIG, cfg->awg|cfg->vbusct|cfg->vshct|cfg->mode);
}

int ina226GetBusVoltage(int channel, unsigned char address, int *voltage)
{
  short v;
  int rc = inaReadRegister(channel, address, INA_REG_VBUS, (unsigned short*)&v);
  if (!rc)
    *voltage = v * 125 / 100;
  return rc;
}

int ina226GetShuntCurrent(int channel, unsigned char address, int R, int *current)
{
  short v;
  int rc = inaReadRegister(channel, address, INA_REG_VSHUNT, (unsigned short*)&v);
  if (!rc)
    *current = v * 2500 / R;
  return rc;
}
