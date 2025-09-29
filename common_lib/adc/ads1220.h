#ifndef ADC_ADS1220_H
#define ADC_ADS1220_H

#define ADS1220_REGISTER_CONFIG0 0
#define ADS1220_REGISTER_CONFIG1 1
#define ADS1220_REGISTER_CONFIG2 2
#define ADS1220_REGISTER_CONFIG3 3

#define ADS1220_COMMAND_RESET     6
#define ADS1220_COMMAND_START     8
#define ADS1220_COMMAND_POWERDOWN 2
#define ADS1220_COMMAND_RDATA     0x10
#define ADS1220_COMMAND_RREG      0x20
#define ADS1220_COMMAND_WREG      0x40

#define ADS1220_MUX_AIN0_AIN1 0
#define ADS1220_MUX_AIN0_AIN2 1
#define ADS1220_MUX_AIN0_AIN3 2
#define ADS1220_MUX_AIN1_AIN2 3
#define ADS1220_MUX_AIN1_AIN3 4
#define ADS1220_MUX_AIN2_AIN3 5
#define ADS1220_MUX_AIN1_AIN0 6
#define ADS1220_MUX_AIN3_AIN2 7
#define ADS1220_MUX_AIN0_AVSS 8
#define ADS1220_MUX_AIN1_AVSS 9
#define ADS1220_MUX_AIN2_AVSS 10
#define ADS1220_MUX_AIN3_AVSS 11
#define ADS1220_MUX_VREF_DIV4 12
#define ADS1220_MUX_AVDD_DIV4 13
#define ADS1220_MUX_SHORT     14
#define ADS1220_MAX_MUX       14

#define ADS1220_PGA_GAIN_1   0
#define ADS1220_PGA_GAIN_2   1
#define ADS1220_PGA_GAIN_4   2
#define ADS1220_PGA_GAIN_8   3
#define ADS1220_PGA_GAIN_16  4
#define ADS1220_PGA_GAIN_32  5
#define ADS1220_PGA_GAIN_64  6
#define ADS1220_PGA_GAIN_128 7
#define ADS1220_MAX_GAIN     7

#define ADS1220_DATA_RATE_20   0
#define ADS1220_DATA_RATE_45   1
#define ADS1220_DATA_RATE_90   2
#define ADS1220_DATA_RATE_175  3
#define ADS1220_DATA_RATE_330  4
#define ADS1220_DATA_RATE_600  5
#define ADS1220_DATA_RATE_1000 6

#define ADS1220_MODE_NORMAL     0
#define ADS1220_MODE_DUTY_CYCLE 1
#define ADS1220_MODE_TURBO      2

#define ADS1220_VREF_INTERNAL  0
#define ADS1220_VREF_EXTERNAL0 1
#define ADS1220_VREF_EXTERNAL1 2
#define ADS1220_VREF_AVDD      3

#define ADS1220_FILTER_OFF   0
#define ADS1220_FILTER_50_60 1
#define ADS1220_FILTER_50    2
#define ADS1220_FILTER_60    3

#define ADS1220_IDAC_OFF  0
#define ADS1220_IDAC_10   1
#define ADS1220_IDAC_50   2
#define ADS1220_IDAC_100  3
#define ADS1220_IDAC_250  4
#define ADS1220_IDAC_500  5
#define ADS1220_IDAC_1000 6
#define ADS1220_IDAC_1500 7

#define ADS1220_IDAC_MUX_OFF   0
#define ADS1220_IDAC_MUX_AIN0  1
#define ADS1220_IDAC_MUX_AIN1  2
#define ADS1220_IDAC_MUX_AIN2  3
#define ADS1220_IDAC_MUX_AIN3  4
#define ADS1220_IDAC_MUX_REFP0 5
#define ADS1220_IDAC_MUX_REFN0 6

typedef struct
{
  unsigned char mux;
  unsigned char gain;
  unsigned char data_rate;
  unsigned char mode;
  unsigned char vref;
  unsigned char filter_50_60;
  unsigned char idac_current;
  unsigned char idac1_mux;
  unsigned char idac2_mux;
  int data_ready_on_dout;
  int auto_close_low_side_switch;
  int continuous_conversion_mode;
  int temperature_sensor_mode;
  int burnout_sources_enable;
  int pga_bypass;
} ads1220_configuration;

typedef struct
{
  int channel;
  unsigned char channel_no;
  unsigned char gain;
  int pga_bypass;
  double input_divider_coefficient;
  double vref;
} ads1220_read_voltage_configuration;

typedef struct
{
  int value;
  double voltage;
} ads1220_data;

void ads1220_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength); // should be defined in hal.c

void ads1220_reset(int channel);
void ads1220_init(void);
void ads1220_set_configuration(int channel, const ads1220_configuration *config);
void ads1220_write_registers(int channel, unsigned char reg_no, const unsigned char *data, unsigned char length);
void ads1220_read_registers(int channel, unsigned char reg_no, unsigned char *data, unsigned char length);
int ads1220_read(int channel, unsigned char channel_no, unsigned char gain, int pga_bypass, int *result, int timeout);
int ads1220_read_voltage(const ads1220_read_voltage_configuration * configuration,
                        ads1220_data *result, int timeout);
void ads1220_set_idac(int channel, unsigned char idac_current, unsigned char idac1_mux, unsigned char idac2_mux);
int ads1220_read_temperature(int channel, ads1220_data *result, int timeout);
int ads1220_calibrate_offset(int channel, unsigned char channel_no, int steps);

void ads1220_read_start(int channel, unsigned char channel_no, unsigned char gain, int pga_bypass);
void ads1220_read_finish(int *result);
void ads1220_read_voltage_finish(const ads1220_read_voltage_configuration * configuration,
                                  ads1220_data *result);

extern int ads1220_offsets[ADS1220_MAX_CHANNELS][ADS1220_MAX_MUX+1][ADS1220_MAX_GAIN+1];

#endif