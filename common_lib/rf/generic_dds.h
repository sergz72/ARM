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

#define DDS_DEVICE_ID 1

#define DDS_AD9833  1
#define DDS_SI5351  2
#define DDS_AD9850  3
#define DDS_ADF4351 4
#define LEVEL_METER_NONE   0
#define LEVEL_METER_AD8307 1
#define LEVEL_METER_AD8310 2
#define LEVEL_METER_AD8313 3
#define LEVEL_METER_AD8317 4
#define LEVEL_METER_AD8318 5

typedef struct __attribute__((packed))
{
  unsigned short type;
  unsigned short level_meter_type;
  unsigned long long int mclk;
  unsigned short max_mv;
  unsigned char max_attenuator;
} DdsConfig;

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
  unsigned char parameter;
} dds_cmd1;

typedef struct __attribute__((__packed__)) {
  unsigned long long int freq;
  unsigned short div;
} dds_cmd10;

typedef struct __attribute__((__packed__)) {
  unsigned long long int freq;
  unsigned int step;
  unsigned int points;
  unsigned short div;
} dds_cmd18;


typedef struct __attribute__((__packed__)) {
  unsigned char command;
  unsigned char channel;
  union
  {
    dds_cmd1 c1;
    dds_cmd10 c10;
    dds_cmd18 c18;
  };
} dds_i2c_command;

#endif
