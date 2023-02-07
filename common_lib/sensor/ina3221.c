#include <ina3221.h>

int ina3221SetConfig(int channel, unsigned char address, const INA3221Config *cfg)
{
  unsigned short config = cfg->base_config.awg|cfg->base_config.vbusct|cfg->base_config.vshct|cfg->base_config.mode;
  unsigned short en = 0x4000;
  int i;
  for (i = 0; i < 3; i++)
  {
    if (cfg->channel_enable[i])
      config |= en;
    en >>= 1;
  }
  return inaWriteRegister(channel, address, INA_REG_CONFIG, config);
}

int ina3221GetBusVoltage(int i2cchannel, unsigned char address, int channel, int *voltage)
{
  short v;
  int rc = inaReadRegister(i2cchannel, address, INA_REG_VBUS + 2 * channel, (unsigned short*)&v);
  if (!rc)
    *voltage = v;
  return rc;
}

int ina3221GetShuntCurrent(int i2cchannel, unsigned char address, int R, int channel, int *current)
{
  short v;
  int rc = inaReadRegister(i2cchannel, address, INA_REG_VSHUNT + 2 * channel, (unsigned short*)&v);
  if (!rc)
    *current = v * 5000 / R;
  return rc;
}
