#ifndef _VEML7700_H
#define _VEML7700_H

#define VEML7700_I2C_ADDRESS 0x10

#define VEML7700_GAIN_1   0
#define VEML7700_GAIN_2   (1 << 11)
#define VEML7700_GAIN_1d8 (2 << 11)
#define VEML7700_GAIN_1d4 (3 << 11)

#define VEML7700_IT_25ms  (0x0C<<6)
#define VEML7700_IT_50ms  (8<<6)
#define VEML7700_IT_100ms 0
#define VEML7700_IT_200ms (1<<6)
#define VEML7700_IT_400ms (2<<6)
#define VEML7700_IT_800ms (3<<6)

#define VEML7700_INT_ENABLE 2
#define VEML7700_SHUTDOWN   1

#define VEML7700_PSM_ENABLE 1
#define VEML7700_PSM_MODE1  0
#define VEML7700_PSM_MODE2  2
#define VEML7700_PSM_MODE3  4
#define VEML7700_PSM_MODE4  6

#define VEML7700_REG_CONFIG         0
#define VEML7700_REG_HIGH_THRESHOLD 1
#define VEML7700_REG_LOW_THRESHOLD  2
#define VEML7700_REG_POWER_SAVE     3
#define VEML7700_REG_ALS            4
#define VEML7700_REG_WHITE          5
#define VEML7700_REG_IT_STATUS      6
#define VEML7700_REG_ID             7

#define VEML7700_INT_TH_LOW  0x8000
#define VEML7700_INT_TH_HIGH 0x4000

typedef struct
{
  float lux;
  unsigned int gainx8;
  unsigned int tries;
} veml7700_result;

int veml7700_read(unsigned char reg, unsigned short *data); // should be defined in hal.c
int veml7700_write(unsigned char reg, unsigned short data); // should be defined in hal.c

int veml7700_init(void);
int veml7700_ex_init(unsigned short config, unsigned short power_save);
int veml7700_measure(veml7700_result *result, int wait);

#endif
