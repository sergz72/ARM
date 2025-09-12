#ifndef ADC_ADS1259_H
#define ADC_ADS1259_H

#define ADS1259_DATA_RATE_10    0
#define ADS1259_DATA_RATE_16p6  1
#define ADS1259_DATA_RATE_50    2
#define ADS1259_DATA_RATE_60    3
#define ADS1259_DATA_RATE_400   4
#define ADS1259_DATA_RATE_1200  5
#define ADS1259_DATA_RATE_3600  6
#define ADS1259_DATA_RATE_14400 7

#define ADS1259_CONVERSION_DELAY_0    0
#define ADS1259_CONVERSION_DELAY_64   1
#define ADS1259_CONVERSION_DELAY_128  2
#define ADS1259_CONVERSION_DELAY_256  3
#define ADS1259_CONVERSION_DELAY_512  4
#define ADS1259_CONVERSION_DELAY_1024 5
#define ADS1259_CONVERSION_DELAY_2048 6
#define ADS1259_CONVERSION_DELAY_4096 7

typedef struct
{
  int channel;
  int expected_value;
  int num_loops;
  int fsc;
  int result;
  double expected_voltage;
  double input_divider_coefficient;
  double vref;
} ads1259_gain_calibration;

typedef struct
{
  int internal_reference_bias_enable;
  int spi_timeout_enable;
  int out_of_range_flag_enable;
  int checksum_enable;
  int digital_filter_sinc2;
  int external_reference_enable;
  unsigned char conversion_delay;
  int syncout_enable;
  int pulse_mode;
  unsigned char data_rate;
} ads1259_configuration;

void ads1259_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength); // should be defined in hal.c

void ads1259_reset(void);
void ads1259_init(int channel, const ads1259_configuration *config);
int ads1259_read(int channel, int *pvalue, int timeout);
int ads1259_calibrate_offset2(int channel, int timeout);
int ads1259_calibrate_offset(int channel, int num_loops, int timeout);
int ads1259_calibrate_gain(ads1259_gain_calibration *calibration, int timeout);
int ads1259_read_voltage(int channel, double input_divider_coefficient, double vref, double *result, int timeout);
void ads1259_set_fsc(int channel, int fsc);

#endif
