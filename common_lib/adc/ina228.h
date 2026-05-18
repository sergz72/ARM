#ifndef _INA228_H
#define _INA228_H

typedef union {
  unsigned short raw;
  struct
  {
    unsigned short reserved:4;
    unsigned short adcrange:1;
    unsigned short tempcomp:1;
    unsigned short convdly:8;
    unsigned short rstacc:1;
    unsigned short rst:1;
  } bits;
} INA228Config;

typedef union {
  unsigned short raw;
  struct
  {
    unsigned short awg:3;
    unsigned short vtct:3;
    unsigned short vshct:3;
    unsigned short vbusct:3;
    unsigned short mode:4;
  } bits;
} INA228AdcConfig;

#define INA228_MODE_POWERDOWN 0
#define INA228_MODE_BUS_TRIG 1
#define INA228_MODE_SHUNT_TRIG 2
#define INA228_MODE_SHUNT_BUS_TRIG 3
#define INA228_MODE_TEMP_TRIG 4
#define INA228_MODE_TEMP_BUS_TRIG 5
#define INA228_MODE_TEMP_SHUNT_TRIG 6
#define INA228_MODE_TEMP_SHUNT_BUS_TRIG 7
#define INA228_MODE_BUS_CONT 9
#define INA228_MODE_SHUNT_CONT 10
#define INA228_MODE_SHUNT_BUS_CONT 11
#define INA228_MODE_TEMP_CONT 12
#define INA228_MODE_TEMP_BUS_CONT 13
#define INA228_MODE_TEMP_SHUNT_CONT 14
#define INA228_MODE_TEMP_SHUNT_BUS_CONT 15

#define INA228_CT_50 0
#define INA228_CT_84 1
#define INA228_CT_150 2
#define INA228_CT_280 3
#define INA228_CT_540 4
#define INA228_CT_1052 5
#define INA228_CT_2074 6
#define INA228_CT_4120 7

#define INA228_AVG_1 0
#define INA228_AVG_4 1
#define INA228_AVG_16 2
#define INA228_AVG_64 3
#define INA228_AVG_128 4
#define INA228_AVG_256 5
#define INA228_AVG_512 6
#define INA228_AVG_1024 7

#define INA228_REG_CONFIG 0
#define INA228_REG_ADC_CONFIG 1
#define INA228_REG_SHUNT_CAL 2
#define INA228_REG_SHUNT_TEMPCO 3
#define INA228_REG_VSHUNT 4
#define INA228_REG_VBUS   5
#define INA228_REG_DIETEMP 6
#define INA228_REG_CURRENT 7
#define INA228_REG_POWER 8
#define INA228_REG_ENERGY 9
#define INA228_REG_CHARGE 10
#define INA228_REG_DIAG_ALRT 11
#define INA228_REG_SOVL 12
#define INA228_REG_SUVL 13
#define INA228_REG_BOVL 14
#define INA228_REG_BUVL 15
#define INA228_REG_TEMP_LIMIT 16
#define INA228_REG_PWR_LIMIT 17
#define INA228_REG_MANUFACTURER_ID 0x3E
#define INA228_REG_DEVICE_ID 0x3F


int ina228ReadRegister16(int channel, unsigned char address, unsigned char reg, unsigned short *data);
int ina228WriteRegister16(int channel, unsigned char address, unsigned char reg, unsigned short data);
int ina228ReadRegister24(int channel, unsigned char address, unsigned char reg, unsigned int *data);
int ina228WriteRegister24(int channel, unsigned char address, unsigned char reg, unsigned int data);
int ina228SetConfig(int channel, unsigned char address, INA228Config cfg);
int ina228SetAdcConfig(int channel, unsigned char address, INA228AdcConfig cfg);
// bus voltage is in uV
int ina228GetBusVoltage(int channel, unsigned char address, int *voltage);
// shunt current is in nA, R is in mOhm
int ina228GetShuntCurrent(int channel, unsigned char address, int R, int *current);
// temperature in mgrdC
int ina228GetTemperature(int channel, unsigned char address, int *temperature);

#endif
