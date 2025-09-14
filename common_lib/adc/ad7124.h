#ifndef ADC_AD7124_H
#define ADC_AD7124_H

#define AD7124_NUM_ADC_CHANNELS     16
#define AD7124_NUM_CONFIGURATIONS   8

#define AD7124_REGISTER_STATUS      0
#define AD7124_REGISTER_ADC_CONTROL 1
#define AD7124_REGISTER_DATA        2
#define AD7124_REGISTER_IO_CONTROL1 3
#define AD7124_REGISTER_IO_CONTROL2 4
#define AD7124_REGISTER_ID          5
#define AD7124_REGISTER_ERROR       6
#define AD7124_REGISTER_ERROR_EN    7
#define AD7124_REGISTER_MCLK_COUNT  8
#define AD7124_REGISTER_CHANNEL0    9
#define AD7124_REGISTER_CONFIG0     0x19
#define AD7124_REGISTER_FILTER0     0x21
#define AD7124_REGISTER_OFFSET0     0x29
#define AD7124_REGISTER_GAIN0       0x31

#define AD7124_ID 0x17

#define AD7124_MODE_CONTINUOUS_CONVERSION       0
#define AD7124_MODE_SINGLE_CONVERSION           (1<<2)
#define AD7124_MODE_STANDBY                     (2<<2)
#define AD7124_MODE_POWER_DOWN                  (3<<2)
#define AD7124_MODE_IDLE                        (4<<2)
#define AD7124_MODE_INTERNAL_OFFSET_CALIBRATION (5<<2)
#define AD7124_MODE_INTERNAL_GAIN_CALIBRATION   (6<<2)
#define AD7124_MODE_SYSTEM_OFFSET_CALIBRATION   (7<<2)
#define AD7124_MODE_SYSTEM_GAIN_CALIBRATION     (8<<2)

#define AD7124_BURNOUT_OFF   0
#define AD7124_BURNOUT_0p5uA 1
#define AD7124_BURNOUT_2uA   2
#define AD7124_BURNOUT_4uA   3

#define AD7124_REFSEL_REFIN1   0
#define AD7124_REFSEL_REFIN2   1
#define AD7124_REFSEL_INTERNAL 2
#define AD7124_REFSEL_AVDD     3

#define AD7124_PGA_GAIN_1   0
#define AD7124_PGA_GAIN_2   1
#define AD7124_PGA_GAIN_4   2
#define AD7124_PGA_GAIN_8   3
#define AD7124_PGA_GAIN_16  4
#define AD7124_PGA_GAIN_32  5
#define AD7124_PGA_GAIN_64  6
#define AD7124_PGA_GAIN_128 7

#define AD7124_FILTER_SINC4       0
#define AD7124_FILTER_SINC3       2
#define AD7124_FILTER_FAST_SINC4  4
#define AD7124_FILTER_FAST_SINC3  5
#define AD7124_FILTER_POST_FILTER 7

#define AD7124_POST_FILTER_47DB   2
#define AD7124_POST_FILTER_62DB   3
#define AD7124_POST_FILTER_86DB   5
#define AD7124_POST_FILTER_92DB   6

#define AD7124_LOW_POWER  0
#define AD7124_MID_POWER  1
#define AD7124_FULL_POWER 3

#define AD7124_CLOCK_INTERNAL_NO_CLK 0
#define AD7124_CLOCK_INTERNAL_CLK    1
#define AD7124_CLOCK_EXTERNAL_CLK614 2
#define AD7124_CLOCK_EXTERNAL        3

#define AD7124_IOUT_OFF    0
#define AD7124_IOUT_50uA   1
#define AD7124_IOUT_100uA  2
#define AD7124_IOUT_250uA  3
#define AD7124_IOUT_500uA  4
#define AD7124_IOUT_750uA  5
#define AD7124_IOUT_1000uA 6
#define AD7124_IOUT_0p1uA  7

#define AD7124_AIN0 0
#define AD7124_AIN1 1
#define AD7124_AIN2 2
#define AD7124_AIN3 3
#define AD7124_AIN4 4
#define AD7124_AIN5 5
#define AD7124_AIN6 6
#define AD7124_AIN7 7
#define AD7124_AIN8 8
#define AD7124_AIN9 9
#define AD7124_AIN10 10
#define AD7124_AIN11 11
#define AD7124_AIN12 12
#define AD7124_AIN13 13
#define AD7124_AIN14 14
#define AD7124_AIN15 15
#define AD7124_TEMP_SENSOR 16
#define AD7124_AVSS 17
#define AD7124_INTERNAL_REFERENCE 18
#define AD7124_DGND 19
#define AD7124_AVDDAVSS6P 20
#define AD7124_AVDDAVSS6M 21
#define AD7124_IOVDDDGND6P 22
#define AD7124_IOVDDDGND6M 23
#define AD7124_ALDOAVSS6P 24
#define AD7124_ALDOAVSS6M 25
#define AD7124_DLDODGND6P 26
#define AD7124_DLDODGND6M 27
#define AD7124_20mVP 28
#define AD7124_20mVM 29

typedef struct
{
  int enable;
  unsigned char setup_no;
  unsigned char ainp;
  unsigned char ainm;
} ad7124_channel_configuration;

typedef struct
{
  int bipolar;
  int ref_bufp;
  int ref_bufm;
  int ain_bufp;
  int ain_bufm;
  int filter_rej60;
  int filter_single_cycle;
  unsigned short filter_fs;
  unsigned char burnout;
  unsigned char refsel;
  unsigned char pga;
  unsigned char filter_type;
  unsigned char post_filter;
} ad7124_configuration;

typedef struct
{
  int channel;
  int expected_value;
  int num_loops;
  int gain;
  int result;
  double expected_voltage;
  double input_divider_coefficient;
  double vref;
  unsigned char channel_no;
  unsigned char config_no;
} ad7124_gain_calibration;

typedef struct
{
  int dout_rdy_del;
  int cont_read;
  int data_status;
  int cs_en;
  int ref_en;
  unsigned char power_mode;
  unsigned char mode;
  unsigned char clk_sel;
} ad7124_adc_control;

typedef struct
{
  unsigned char gpio_dat;
  unsigned char gpio_ctrl;
  unsigned char iout0;
  unsigned char iout1;
  unsigned char iout0_ch;
  unsigned char iout1_ch;
  unsigned short vbias;
  int pdsw;
} ad7124_io_control;

typedef struct
{
  int mclk_cnt_en;
  int ldo_cap_chk_test_en;
  int adc_cal_err_en;
  int adc_conv_err_en;
  int adc_sat_err_en;
  int ainp_ov_err_en;
  int ainp_uv_err_en;
  int ainm_ov_err_en;
  int ainm_uv_err_en;
  int ref_det_err_en;
  int dldo_psm_trip_test_en;
  int dldo_psm_err_en;
  int aldo_psm_trip_test_en;
  int aldo_psm_err_en;
  int spi_ignore_err_en;
  int spi_sclk_cnt_err_en;
  int spi_read_err_en;
  int spi_write_err_en;
  int spi_crc_err_en;
  int mm_crc_err_en;
  int rom_crc_err_en;
  unsigned char ldo_cap_chk;
} ad7124_error_control;

typedef struct
{
  int channel;
  int value;
  double voltage;
} ad7124_data;

void ad7124_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength); // should be defined in hal.c

void ad7124_init(void);
void ad7124_write_register(int channel, unsigned char reg_no, const unsigned char *data, int length);
void ad7124_read_register(int channel, unsigned char reg_no, unsigned char *data, int length);
void ad7124_setup_channel(int channel, unsigned char channel_no, const ad7124_channel_configuration *config);
void ad7124_setup_configuration(int channel, unsigned char config_no, const ad7124_configuration *config);
void ad7124_setup_adc_control(int channel, const ad7124_adc_control *config);
void ad7124_setup_io_control(int channel, const ad7124_io_control *config);
void ad7124_setup_error_control(int channel, const ad7124_error_control *config);
int ad7124_read(int channel, ad7124_data *result, unsigned int channels, int timeout);
int ad7124_read_voltage(int channel, unsigned int bipolar_channels, double input_divider_coefficient, double vref,
                        ad7124_data *result, unsigned int channels, int timeout);
int ad7124_calibrate_offset(int channel, unsigned char channel_no, unsigned char config_no, int num_loops, int timeout);
int ad7124_calibrate_gain(ad7124_gain_calibration *calibration, int timeout);
void ad7124_set_mode(int channel, unsigned char mode);
int ad7124_wait(int channel, int timeout);
int ad7124_calibrate_offset_internal(int channel, unsigned char channel_no, int timeout);
int ad7124_calibrate_gain_internal(int channel, unsigned char channel_no, int timeout);
int ad7124_calibrate_offset_system(int channel, unsigned char channel_no, int timeout);
int ad7124_calibrate_gain_system(int channel, unsigned char channel_no, int timeout);
int ad7124_get_offset(int channel, unsigned char channel_no);
void ad7124_set_offset(int channel, unsigned char channel_no, int value);
int ad7124_get_gain(int channel, unsigned char channel_no);
void ad7124_set_gain(int channel, unsigned char channel_no, int value);

#endif
