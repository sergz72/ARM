#ifndef _MULTIMETER_H
#define _MULTIMETER_H

#define DAC_LEVEL_0   1
#define DAC_LEVEL_1   14
#define DAC_LEVEL_2   140
#define DAC_LEVEL_3   1400
#define DAC_MAX_LEVEL 3

typedef struct
{
  unsigned int v[DAC_MAX_LEVEL+1][ADC_MAX_GAIN+1];
  unsigned int r[DAC_MAX_LEVEL+1][ADC_MAX_GAIN+1];
} calibration_data_t;

int calculateR(unsigned long long int *value);
int calibrateR(int level, int gain, int R, int *result);

extern const calibration_data_t *calibration_data;

#endif
