#ifndef _MULTIMETER_H
#define _MULTIMETER_H

#define MAX_LEVEL 4

#define DAC_LEVEL_0   1
#define DAC_LEVEL_1   14
#define DAC_LEVEL_2   140
#define DAC_LEVEL_3   1400
#define DAC_LEVEL_4   1400

#define ADC_GAIN_LEVEL_0 0
#define ADC_GAIN_LEVEL_1 0
#define ADC_GAIN_LEVEL_2 0
#define ADC_GAIN_LEVEL_3 0
#define ADC_GAIN_LEVEL_4 3

typedef struct
{
  unsigned int v;
  unsigned int r;
} calibration_data_vr;

typedef struct
{
  calibration_data_vr vr[MAX_LEVEL+1];
} calibration_data_t;

typedef struct
{
  unsigned long long int r;
  int level;
} getr_result;

int calculateR(getr_result *value);
int calibrateR(int level, int R, int *result);

extern const calibration_data_t *calibration_data;
extern const unsigned int dac_levels[MAX_LEVEL+1];
extern const int adc_gains[MAX_LEVEL+1];

#endif
