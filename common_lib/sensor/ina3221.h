#ifndef _INA3221_H
#define _INA3221_H

#include <ina226.h>

typedef struct {
  int channel_enable[3];
  INA226Config base_config;
} INA3221Config;

int ina3221SetConfig(int channel, unsigned char address, const INA3221Config *cfg);
// bus voltage is in mV
int ina3221GetBusVoltage(int i2cchannel, unsigned char address, int channel, int *voltage);
// shunt current is in uA, R is in mOhm
int ina3221GetShuntCurrent(int i2cchannel, unsigned char address, int R, int channel, int *current);

#endif
