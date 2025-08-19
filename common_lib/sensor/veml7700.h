#ifndef _VEML7700_H
#define _VEML7700_H

#define VEML7700_I2C_ADDRESS 0x10

typedef struct
{
  float lux;
  unsigned int gainx8;
  unsigned int tries;
} veml7700_result;

int veml7700_read(unsigned char reg, unsigned short *data); // should be defined in hal.c
int veml7700_write(unsigned char reg, unsigned short data); // should be defined in hal.c

int veml7700_init(void);
int veml7700_measure(veml7700_result *result);

#endif
