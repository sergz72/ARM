#ifndef _INA226_H
#define _INA226_H

typedef struct {
  unsigned short awg;
  unsigned short vbusct;
  unsigned short vshct;
  unsigned short mode;
} INA226Config;

#define INA_MODE_POWERDOWN 0
#define INA_MODE_SHUNT_TRIG 1
#define INA_MODE_BUS_TRIG 2
#define INA_MODE_SHUNT_BUS_TRIG 3
#define INA_MODE_SHUNT_CONT 5
#define INA_MODE_BUS_CONT 6
#define INA_MODE_SHUNT_BUS_CONT 7

#define INA_VSHCT_140 0
#define INA_VSHCT_204 (1 << 3)
#define INA_VSHCT_332 (2 << 3)
#define INA_VSHCT_588 (3 << 3)
#define INA_VSHCT_1100 (4 << 3)
#define INA_VSHCT_2216 (5 << 3)
#define INA_VSHCT_4156 (6 << 3)
#define INA_VSHCT_8244 (7 << 3)

#define INA_VBUSCT_140 0
#define INA_VBUSCT_204 (1 << 6)
#define INA_VBUSCT_332 (2 << 6)
#define INA_VBUSCT_588 (3 << 6)
#define INA_VBUSCT_1100 (4 << 6)
#define INA_VBUSCT_2216 (5 << 6)
#define INA_VBUSCT_4156 (6 << 6)
#define INA_VBUSCT_8244 (7 << 6)

#define INA_AVG_1 0
#define INA_AVG_4 (1 << 9)
#define INA_AVG_16 (2 << 9)
#define INA_AVG_64 (3 << 9)
#define INA_AVG_128 (4 << 9)
#define INA_AVG_256 (5 << 9)
#define INA_AVG_512 (6 << 9)
#define INA_AVG_1024 (7 << 9)

#define INA_REG_CONFIG 0
#define INA_REG_VSHUNT 1
#define INA_REG_VBUS   2

int inaReadRegister(int channel, unsigned char address, unsigned char reg, unsigned short *data);
int inaWriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data);
int inaReset(int channel, unsigned char address);
int ina226SetConfig(int channel, unsigned char address, const INA226Config *cfg);
// bus voltage is in mV
int ina226GetBusVoltage(int channel, unsigned char address, int *voltage);
// shunt current is in uA, R is in mOhm
int ina226GetShuntCurrent(int channel, unsigned char address, int R, int *current);

#endif
