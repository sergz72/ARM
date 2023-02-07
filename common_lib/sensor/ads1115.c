#include <ads1115.h>

#define ADS1115_REG_CONFIG 1
#define ADS1115_REG_CONV   0

int ads1115SetConfig(int channel, unsigned char address, const ADS1115Config *cfg)
{
  unsigned short config = cfg->pga | cfg->mux | cfg->dr | cfg->comp_que;
  if (cfg->comp_latching)
    config |= 4;
  if (cfg->comp_mode_window)
    config |= 0x10;
  if (cfg->comp_active_high)
    config |= 8;
  if (cfg->single_shot_mode)
    config |= 0x100;
  if (cfg->os)
    config |= 0x8000;
  return ads1115WriteRegister(channel, address, ADS1115_REG_CONFIG, config);
}

int ads1115GetVoltage(int channel, unsigned char address, unsigned short pga, int *voltage)
{
  short v;
  long long int vv;
  int rc = ads1115ReadRegister(channel, address, ADS1115_REG_CONV, (unsigned short*)&v);
  if (!rc)
  {
    switch (pga)
    {
      case ADS1115_PGA_6144:
        vv = v * 6144000 / 32768;
        break;
      case ADS1115_PGA_4096:
        vv = v * 4096000 / 32768;
        break;
      case ADS1115_PGA_2048:
        vv = v * 2048000 / 32768;
        break;
      case ADS1115_PGA_1024:
        vv = v * 1024000 / 32768;
        break;
      case ADS1115_PGA_512:
        vv = v * 512000 / 32768;
        break;
      default:
        vv = v * 256000 / 32768;
        break;
    }
    *voltage = (int)vv;
  }
  return rc;
}
