#ifndef _BH1750_H
#define _BH1750_H

/// Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
#define BH1750_ONE_TIME_HIGH_RES_MODE_2 0x21
#define BH1750_MTREG_MAX 254
#define BH1750_MTREG_MID 69
#define BH1750_MTREG_MIN 31

int bh1750_write(unsigned char command); // should be defined in hal.c
int bh1750_read(unsigned char *result); // should be defined in hal.c

int bh1750_measure(unsigned char mode, unsigned short *result);
int bh1750_set_measurement_time(unsigned char mtreg);

#endif
