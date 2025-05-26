#ifndef TLV320DAC3100_H
#define TLV320DAC3100_H

#define TLV320DAC3100_I2C_ADDRESS 0x30

#define TLV320DAC3100_REGISTER_PAGE                   0

//page 0
#define TLV320DAC3100_REGISTER_SOFT_RESET             1
#define TLV320DAC3100_REGISTER_OT_FLAG                3
#define TLV320DAC3100_REGISTER_CLOCK_MUX              4
#define TLV320DAC3100_REGISTER_PLL_PR                 5
#define TLV320DAC3100_REGISTER_PLL_J                  6
#define TLV320DAC3100_REGISTER_PLL_D_MSB              7
#define TLV320DAC3100_REGISTER_PLL_D_LSB              8
#define TLV320DAC3100_REGISTER_NDAC                   11
#define TLV320DAC3100_REGISTER_MDAC                   12
#define TLV320DAC3100_REGISTER_DOSR_MSB               13
#define TLV320DAC3100_REGISTER_DOSR_LSB               14
#define TLV320DAC3100_REGISTER_CLKOUT_MUX             25
#define TLV320DAC3100_REGISTER_CLKOUT_M               26
#define TLV320DAC3100_REGISTER_INTERFACE_CONTROL1     27
#define TLV320DAC3100_REGISTER_DATA_OFFSET            28
#define TLV320DAC3100_REGISTER_INTERFACE_CONTROL2     29
#define TLV320DAC3100_REGISTER_BCLK_N                 30
#define TLV320DAC3100_REGISTER_SEC_INTERFACE_CONTROL1 31
#define TLV320DAC3100_REGISTER_SEC_INTERFACE_CONTROL2 32
#define TLV320DAC3100_REGISTER_SEC_INTERFACE_CONTROL3 33
#define TLV320DAC3100_REGISTER_I2C_BUS_CONDITION      34
#define TLV320DAC3100_REGISTER_DAC_FLAGS1             37
#define TLV320DAC3100_REGISTER_DAC_FLAGS2             38
#define TLV320DAC3100_REGISTER_OVERFLOW_FLAGS         39
#define TLV320DAC3100_REGISTER_DAC_INTERRUPT_FLAGS1   44
#define TLV320DAC3100_REGISTER_DAC_INTERRUPT_FLAGS2   46
#define TLV320DAC3100_REGISTER_INT1_CONTROL           48
#define TLV320DAC3100_REGISTER_INT2_CONTROL           49
#define TLV320DAC3100_REGISTER_GPIO1_CONTROL          51
#define TLV320DAC3100_REGISTER_DIN_CONTROL            54
#define TLV320DAC3100_REGISTER_DAC_PROCESSING_BLOCK   60
#define TLV320DAC3100_REGISTER_DAC_DATA_PATH          63
#define TLV320DAC3100_REGISTER_DAC_VOLUME             64
#define TLV320DAC3100_REGISTER_DAC_LEFT_VOLUME        65
#define TLV320DAC3100_REGISTER_DAC_RIGHT_VOLUME       66
#define TLV320DAC3100_REGISTER_HEADSET_DETECTION      67
#define TLV320DAC3100_REGISTER_DRC_CONTROL1           68
#define TLV320DAC3100_REGISTER_DRC_CONTROL2           69
#define TLV320DAC3100_REGISTER_DRC_CONTROL3           70
#define TLV320DAC3100_REGISTER_LEFT_BEEP_GEN          71
#define TLV320DAC3100_REGISTER_RIGHT_BEEP_GEN         72
#define TLV320DAC3100_REGISTER_BEEP_LENGTH_MSB        73
#define TLV320DAC3100_REGISTER_BEEP_LENGTH_MID        74
#define TLV320DAC3100_REGISTER_BEEP_LENGTH_LSB        75
#define TLV320DAC3100_REGISTER_BEEP_SIN_MSB           76
#define TLV320DAC3100_REGISTER_BEEP_SIN_LSB           77
#define TLV320DAC3100_REGISTER_BEEP_COS_MSB           78
#define TLV320DAC3100_REGISTER_BEEP_COS_LSB           79
#define TLV320DAC3100_REGISTER_ADC_VOLUME             116
#define TLV320DAC3100_REGISTER_PIN_GAIN               117

// page 1
#define TLV320DAC3100_REGISTER_ERROR_CONTROL          30
#define TLV320DAC3100_REGISTER_HEADPHONE_DRIVERS      31
#define TLV320DAC3100_REGISTER_SPEAKER_AMPLIFIER      32
#define TLV320DAC3100_REGISTER_POP_REMOVAL            33
#define TLV320DAC3100_REGISTER_PGA_RAMP_DOWN          34
#define TLV320DAC3100_REGISTER_MIXER_ROUTING          35
#define TLV320DAC3100_REGISTER_ANALOG_VOLUME_HPL      36
#define TLV320DAC3100_REGISTER_ANALOG_VOLUME_HPR      37
#define TLV320DAC3100_REGISTER_ANALOG_VOLUME_SPK      38
#define TLV320DAC3100_REGISTER_HPL_DRIVER             40
#define TLV320DAC3100_REGISTER_HPR_DRIVER             41
#define TLV320DAC3100_REGISTER_SPK_DRIVER             42
#define TLV320DAC3100_REGISTER_HP_DRIVER              44
#define TLV320DAC3100_REGISTER_MICBIAS                46
#define TLV320DAC3100_REGISTER_INPUT_CM               50

//page 3
#define TLV320DAC3100_REGISTER_TIMER_DIVIDER          16

//page 8 Coefficients RAM buffer A
#define TLV320DAC3100_REGISTER_DAC_RAM_CONTROL        1

//page 9 Coefficients RAM buffer A
//page 12 Coefficients RAM buffer B
//page 13 Coefficients RAM buffer B

#define TLV320DAC3100_INTERFACE_I2S 0
#define TLV320DAC3100_INTERFACE_DSP 0x40
#define TLV320DAC3100_INTERFACE_RJF 0x80
#define TLV320DAC3100_INTERFACE_LJF 0xC0

#define TLV320DAC3100_WORD_LENGTH_16 0
#define TLV320DAC3100_WORD_LENGTH_20 0x10
#define TLV320DAC3100_WORD_LENGTH_24 0x20
#define TLV320DAC3100_WORD_LENGTH_32 0x30

#define TLV320DAC3100_PLL_CLKIN_MCLK  0
#define TLV320DAC3100_PLL_CLKIN_BCLK  4
#define TLV320DAC3100_PLL_CLKIN_GPIO1 8
#define TLV320DAC3100_PLL_CLKIN_DIN   12

#define TLV320DAC3100_CODEC_CLKIN_MCLK  0
#define TLV320DAC3100_CODEC_CLKIN_BCLK  1
#define TLV320DAC3100_CODEC_CLKIN_GPIO1 2
#define TLV320DAC3100_CODEC_CLKIN_PLL   3

typedef struct
{
  unsigned char interface_type;
  unsigned char word_length;
  int bclk_output;
  int wclk_output;
  int bclk_inverted;
  int bclk_wclk_active_in_power_down;
  int bdiv_clkin_eq_dac_clk;
} TLV320DAC3100_InterfaceControl;

#define TLV320DAC3100_DATA_PATH_OFF      0
#define TLV320DAC3100_DATA_PATH_NORMAL   1
#define TLV320DAC3100_DATA_PATH_INVERTED 2
#define TLV320DAC3100_DATA_PATH_MONO     3

#define TLV320DAC3100_SOFT_STEPPING_ONE_STEP_PER_SAMPLE      0
#define TLV320DAC3100_SOFT_STEPPING_ONE_STEP_PER_TWO_SAMPLES 1
#define TLV320DAC3100_SOFT_STEPPING_OFF                      2

typedef struct
{
  int channel_power_up[2];
  unsigned char data_path[2];
  unsigned char soft_stepping;
} TLV320DAC3100_DataPath;

#define TLV320DAC3100_DAC_ROUTING_NONE  0
#define TLV320DAC3100_DAC_ROUTING_MIXER 1
#define TLV320DAC3100_DAC_ROUTING_HP    2

typedef struct
{
  unsigned char dac_routing[2];
  int ain1_left;
  int ain2_left;
  int ain2_right;
  int differential_output_mode;
} TLV320DAC3100_Routing;

#define TLV320DAC3100_VOLUME_CONTROL_INDEPENDENT   0
#define TLV320DAC3100_VOLUME_CONTROL_RIGHT_TO_LEFT 1
#define TLV320DAC3100_VOLUME_CONTROL_LEFT_TO_RIGHT 2

#define TLV320DAC3100_HP_COMMON_MODE_VOLTAGE_1350 0
#define TLV320DAC3100_HP_COMMON_MODE_VOLTAGE_1500 4
#define TLV320DAC3100_HP_COMMON_MODE_VOLTAGE_1650 8
#define TLV320DAC3100_HP_COMMON_MODE_VOLTAGE_1800 12

typedef struct
{
  int driver_power_up[2];
  int short_circuit_protection_enable;
  unsigned char common_mode_voltage;
} TLV320DAC3100_HPControl;

#define TLV320DAC3100_GPIO1_DISABLED 0
#define TLV320DAC3100_GPIO1_CLKOUT   0x10

#define TLV320DAC3100_CLKOUT_SOURCE_MCLK        0
#define TLV320DAC3100_CLKOUT_SOURCE_BCLK        1
#define TLV320DAC3100_CLKOUT_SOURCE_DIN         2
#define TLV320DAC3100_CLKOUT_SOURCE_PLL         3
#define TLV320DAC3100_CLKOUT_SOURCE_DAC_CLK     4
#define TLV320DAC3100_CLKOUT_SOURCE_DAC_MOD_CLK 5

int tlv320dac3100_read(unsigned char register_number, unsigned char *value); //should be defined in hal.c
int tlv320dac3100_write(unsigned char register_number, unsigned char value); //should be defined in hal.c

int tlv320dac3100_init(void);
int tlv320dac3100_set_page(unsigned char page);
int tlv320dac3100_set_interface(const TLV320DAC3100_InterfaceControl *control);
int tlv320dac3100_set_clock(unsigned char pll_clkin, unsigned char codec_clkin);
int tlv320dac3100_set_pll(int enable, unsigned char p, unsigned char r, unsigned char j, unsigned short d);
int tlv320dac3100_set_ndac(int enable, unsigned char divider);
int tlv320dac3100_set_mdac(int enable, unsigned char divider);
int tlv320dac3100_set_dosr(unsigned short value);
int tlv320dac3100_set_datapath(const TLV320DAC3100_DataPath *path);
int tlv320dac3100_set_routing(const TLV320DAC3100_Routing *routing);
int tlv320dac3100_dac_volume_control(int mute_left, int mute_right, unsigned char volume_control);
int tlv320dac3100_dac_volume(int channel, int dbx10);
int tlv320dac3100_set_headphone_driver(const TLV320DAC3100_HPControl *control);
int tlv320dac3100_set_headphone_volume(int channel, int enable, unsigned char value);
int tlv320dac3100_set_headphone_pga(int channel, int unmute, unsigned char gain);
int tlv320dac3100_set_clkout(int enable, unsigned char source, unsigned char divider);

#endif
