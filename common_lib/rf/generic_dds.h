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

typedef struct __attribute__((__packed__)) {
  unsigned long  max_frequency;
  unsigned long  min_frequency;
  unsigned short mclk_MHz;
  unsigned short max_vout_mV;
  unsigned short max_attenuator_value;
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
  unsigned long frequency;
  unsigned short divider;
} dds_set_frequency_command;

typedef struct {
  unsigned long frequency_code;
  unsigned short divider;
} dds_set_frequency_code_command;

typedef struct {
  unsigned short attenuator_value;
} dds_set_attenuator_command;

typedef struct {
  unsigned char channel;
  union {
    dds_enable_output_command enable_command;
    dds_set_mode_command set_mode_command;
    dds_set_frequency_command set_frequency_command;
    dds_set_frequency_code_command set_frequency_code_command;
    dds_set_attenuator_command set_attenuator_command;
  };
} dds_cmd;

typedef struct __attribute__((__packed__)) {
  unsigned char command;
  unsigned char channel;
  unsigned char parameter;
} dds_cmd3;

typedef struct __attribute__((__packed__)) {
  unsigned char command;
  unsigned char channel;
  unsigned short parameter;
} dds_cmd4;

typedef struct __attribute__((__packed__)) {
  unsigned char  command;
  unsigned char  channel;
  unsigned long  freq;
  unsigned short div;
} dds_cmd8;

typedef union __attribute__((__packed__)) {
    unsigned char bytes[8];
    dds_cmd3 c3;
    dds_cmd4 c4;
    dds_cmd8 c8;
} dds_i2c_command;

#endif
