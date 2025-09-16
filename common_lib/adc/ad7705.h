#ifndef ADC_AD7705_H
#define ADC_AD7705_H

#define AD7705_MODE_NORMAL                 0
#define AD7705_MODE_SELF_CALIBRATION       0x40
#define AD7705_MODE_ZERO_SCALE_CALIBRATION 0x80
#define AD7705_MODE_FULL_SCALE_CALIBRATION 0xC0

#define AD7705_PGA_GAIN_1   0
#define AD7705_PGA_GAIN_2   8
#define AD7705_PGA_GAIN_4   2
#define AD7705_PGA_GAIN_8   3
#define AD7705_PGA_GAIN_16  4
#define AD7705_PGA_GAIN_32  5
#define AD7705_PGA_GAIN_64  6
#define AD7705_PGA_GAIN_128 7

#define AD7705_FILTER_50Hz  0
#define AD7705_FILTER_60Hz  1
#define AD7705_FILTER_250Hz 2
#define AD7705_FILTER_500Hz 3

#define AD7705_REGISTER_COMMUNICATION 0
#define AD7705_REGISTER_SETUP         1
#define AD7705_REGISTER_CLOCK         2
#define AD7705_REGISTER_DATA          3
#define AD7705_REGISTER_TEST          4
#define AD7705_REGISTER_NOP           5
#define AD7705_REGISTER_OFFSET        6
#define AD7705_REGISTER_GAIN          7

#define AD7705_CHANNEL_AIN1P_AIN1M 0
#define AD7705_CHANNEL_AIN2P_AIN2M 1
#define AD7705_CHANNEL_AIN1M_AIN1M 2
#define AD7705_CHANNEL_AIN1M_AIN2M 3

typedef struct
{
  unsigned char pga_gain;
  unsigned char filter;
  int unipolar;
  int buf;
  int fsync;
  int clkdis;
  int clkdiv;
  int clk;
} ad7705_configuration;

typedef struct
{
  int value;
  double voltage;
} ad7705_data;

void ad7705_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength); // should be defined in hal.c

void ad7705_init(void);
void ad7705_write_register(int channel, unsigned char reg_no, const unsigned char *data, int length);
void ad7705_read_register(int channel, unsigned char reg_no, unsigned char *data, int length);
void ad7705_set_channel(int channel, unsigned char channel_no);
void ad7705_setup_configuration(int channel, const ad7705_configuration *config);
int ad7705_read(int channel, unsigned char channel_no, int *result, int timeout);
int ad7705_read_voltage(int channel, unsigned char channel_no, double input_divider_coefficient, double vref,
                        ad7705_data *result, int timeout);
int ad7705_wait(int channel, int timeout);
int ad7705_calibration(int channel, unsigned char channel_no, unsigned char mode, int timeout);
int ad7705_get_offset(int channel, unsigned char channel_no);
void ad7705_set_offset(int channel, unsigned char channel_no, int value);
int ad7705_get_gain(int channel, unsigned char channel_no);
void ad7705_set_gain(int channel, unsigned char channel_no, int value);
unsigned char ad7705_get_pga(int channel);
void ad7705_set_pga(int channel, unsigned char value);
void ad7705_start_conversion(int channel, int channel_no);
void ad7705_stop_conversion(int channel);

#endif