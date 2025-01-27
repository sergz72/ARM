#ifndef _GENERIC_DDS_H_
#define _GENERIC_DDS_H_

#define DDS_MODE_SINE        0
#define DDS_MODE_TRIANGLE    1
#define DDS_MODE_SQUARE      2

#define DDS_COMMAND_SET_FREQUENCY      1
#define DDS_COMMAND_SET_FREQUENCY_CODE 2
#define DDS_COMMAND_SET_MODE           3
#define DDS_COMMAND_SET_ATTENUATOR     4
#define DDS_COMMAND_ENABLE_OUTPUT      5
#define DDS_COMMAND_SWEEP              6
#define DDS_COMMAND_SWEEP_CODES        7

typedef struct __attribute__((__packed__)) {
  unsigned long long int max_frequency;
  unsigned long long int min_frequency;
  unsigned short mclk_MHz;
  unsigned short max_vout_mV;
  unsigned char  max_attenuator_value;
  unsigned char  channels;
  unsigned char  accumulator_bits;
  unsigned char  out_square_divider_bits;
  unsigned char  supported_modes;
} dds_config;

typedef struct {
  unsigned char enable;
} dds_enable_output_command;

typedef struct {
  unsigned char mode;
} dds_set_mode_command;

typedef struct {
  unsigned long long int frequency;
  unsigned short divider;
} dds_set_frequency_command;

typedef struct {
  unsigned long long int frequency;
  unsigned int step;
  unsigned short points;
  unsigned short divider;
} dds_sweep_command;

typedef struct {
  unsigned char attenuator_value;
} dds_set_attenuator_command;

typedef struct {
  unsigned char channel;
  union {
    dds_enable_output_command enable_command;
    dds_set_mode_command set_mode_command;
    dds_set_frequency_command set_frequency_command;
    dds_set_attenuator_command set_attenuator_command;
    dds_sweep_command sweep_command;
  };
} dds_cmd;

typedef struct __attribute__((__packed__)) {
  unsigned char command;
  unsigned char channel;
  unsigned char parameter;
} dds_cmd3;

typedef struct __attribute__((__packed__)) {
  unsigned char  command;
  unsigned char  channel;
  unsigned long long int freq;
  unsigned short div;
} dds_cmd12;

typedef struct __attribute__((__packed__)) {
  unsigned char  command;
  unsigned char  channel;
  unsigned long long int freq;
  unsigned int step;
  unsigned int points;
  unsigned short div;
} dds_cmd20;

typedef union __attribute__((__packed__)) {
    unsigned char bytes[20];
    dds_cmd3 c3;
    dds_cmd12 c12;
    dds_cmd20 c20;
} dds_i2c_command;

#endif
