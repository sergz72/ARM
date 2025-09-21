#ifndef ADC_AD7793_H
#define ADC_AD7793_H

#define AD7793_REGISTER_COMMUNICATION 0
#define AD7793_REGISTER_STATUS        0
#define AD7793_REGISTER_MODE          1
#define AD7793_REGISTER_CONFIGURATION 2
#define AD7793_REGISTER_DATA          3
#define AD7793_REGISTER_ID            4
#define AD7793_REGISTER_IO            5
#define AD7793_REGISTER_OFFSET        6
#define AD7793_REGISTER_GAIN          7

#define AD7793_MODE_CONTINUOUS_CONVERSION       0
#define AD7793_MODE_SINGLE_CONVERSION           0x20
#define AD7793_MODE_IDLE                        0x40
#define AD7793_MODE_POWER_DOWN                  0x60
#define AD7793_MODE_INTERNAL_OFFSET_CALIBRATION 0x80
#define AD7793_MODE_INTERNAL_GAIN_CALIBRATION   0xA0
#define AD7793_MODE_SYSTEM_OFFSET_CALIBRATION   0xC0
#define AD7793_MODE_SYSTEM_GAIN_CALIBRATION     0xE0

#define AD7793_UPDATE_RATE_470   1
#define AD7793_UPDATE_RATE_242   2
#define AD7793_UPDATE_RATE_123   3
#define AD7793_UPDATE_RATE_62    4
#define AD7793_UPDATE_RATE_50    5
#define AD7793_UPDATE_RATE_39    6
#define AD7793_UPDATE_RATE_33    7
#define AD7793_UPDATE_RATE_19_90 8
#define AD7793_UPDATE_RATE_16_80 9
#define AD7793_UPDATE_RATE_16_65 10
#define AD7793_UPDATE_RATE_12_66 11
#define AD7793_UPDATE_RATE_10_69 12
#define AD7793_UPDATE_RATE_8_70  13
#define AD7793_UPDATE_RATE_6_72  14
#define AD7793_UPDATE_RATE_4_74  15

#define AD7793_VBIAS_OFF    0
#define AD7793_VBIAS_AIN1M  1
#define AD7793_VBIAS_AIN2M  2

#define AD7793_GAIN_1   0
#define AD7793_GAIN_2   1
#define AD7793_GAIN_4   2
#define AD7793_GAIN_8   3
#define AD7793_GAIN_16  4
#define AD7793_GAIN_32  5
#define AD7793_GAIN_64  6
#define AD7793_GAIN_128 7

#define AD7793_CHANNEL_AIN1P_AIN1N 0
#define AD7793_CHANNEL_AIN2P_AIN2N 1
#define AD7793_CHANNEL_AIN3P_AIN3N 2
#define AD7793_CHANNEL_AIN1N_AIN1N 3
#define AD7793_CHANNEL_TEMP_SENSOR 6
#define AD7793_CHANNEL_AVDD6       7

#define AD7793_IEXCDIR_IEXC1_IOUT1_IEXC2_IOUT2 0
#define AD7793_IEXCDIR_IEXC1_IOUT2_IEXC2_IOUT1 4
#define AD7793_IEXCDIR_BOTH_IOUT1              8
#define AD7793_IEXCDIR_BOTH_IOUT2              0x0C

#define AD7793_IEXCEN_0    0
#define AD7793_IEXCEN_10   1
#define AD7793_IEXCEN_210  2
#define AD7793_IEXCEN_1000 3

typedef struct
{
  unsigned char vbias;
  unsigned char gain;
  unsigned char channel;
  int burnout_enable;
  int unipolar;
  int boost;
  int internal_reference;
  int buffer_enable;
} ad7793_configuration;

typedef struct
{
  int channel;
  unsigned char channel_no;
  unsigned char gain;
  double input_divider_coefficient;
  double vref;
} ad7793_read_voltage_configuration;

typedef struct
{
  int value;
  double voltage;
} ad7793_data;

void ad7793_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength); // should be defined in hal.c

void ad7793_init(void);
void ad7793_write_register(int channel, unsigned char reg_no, const unsigned char *data, int length);
void ad7793_read_register(int channel, unsigned char reg_no, unsigned char *data, int length);
void ad7793_set_mode(int channel, unsigned char mode);
void ad7793_set_mode_and_update_rate(int channel, unsigned char mode, unsigned char update_rate);
void ad7793_set_configuration(int channel, const ad7793_configuration *config);
void ad7793_set_channel_and_gain(int channel, unsigned char channel_no, unsigned char gain);
void ad7793_set_io(int channel, unsigned char iexcdir, unsigned char iexcen);
void ad7793_set_bias(int channel, unsigned char bias);
int ad7793_wait(int channel, int timeout);
int ad7793_calibrate_offset_internal(int channel, unsigned char channel_no, unsigned char gain, int timeout);
int ad7793_calibrate_gain_internal(int channel, unsigned char channel_no, unsigned char gain, int timeout);
int ad7793_calibrate_offset_system(int channel, unsigned char channel_no, unsigned char gain, int timeout);
int ad7793_calibrate_gain_system(int channel, unsigned char channel_no, unsigned char gain, int timeout);
int ad7793_get_offset(int channel, unsigned char channel_no);
void ad7793_set_offset(int channel, unsigned char channel_no, int value);
int ad7793_get_gain(int channel, unsigned char channel_no);
void ad7793_set_gain(int channel, unsigned char channel_no, int value);
int ad7793_read(int channel, unsigned char channel_no, unsigned char gain, int *result, int timeout);
int ad7793_read_voltage(const ad7793_read_voltage_configuration * configuration,
                        ad7793_data *result, int timeout);

#endif
