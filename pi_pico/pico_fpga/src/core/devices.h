#ifndef _DEVICES_H
#define _DEVICES_H

#include "board.h"

#define MEASURE_TYPE_VOLTS 0
#define MEASURE_TYPE_AMPERS 1
#define MEASURE_TYPE_OHMS 2
#define MEASURE_TYPE_FARADS 3
#define MEASURE_TYPE_HENRY 4
#define MEASURE_TYPE_GRADS 5
#define MEASURE_TYPE_HERZ 6
#define MEASURE_TYPE_WATTS 7

#define VALUE_TYPE_ONES 0
#define VALUE_TYPE_MILLIS 1
#define VALUE_TYPE_MICROS 2
#define VALUE_TYPE_NANOS 3
#define VALUE_TYPE_PICOS 4

typedef struct {
  const char *name;
  int device_id;
  int public_id;
  void* (*initializer)(int idx, void **data);
  int (*save_config)(int idx, void *buffer);
  int (*timer_event)(int idx, int step, void *config, void *data, int interrupt, unsigned char *buffer);
  int (*message_processor)(int idx, void *config, void *data, unsigned char *buffer, int len);
} Device;

typedef struct
{
  unsigned char channels;
  unsigned char resolution;
  unsigned char measure_type;
  unsigned char value_type;
  unsigned char numbers_before_point;
} MeterConfig;

typedef struct
{
  unsigned char channels;
  unsigned char voltage_resolution;
  unsigned char voltage_value_type;
  unsigned char voltage_numbers_before_point;
  unsigned char current_resolution;
  unsigned char current_value_type;
  unsigned char current_numbers_before_point;
} PowerMeterConfig;

typedef struct __attribute__((packed))
{
  unsigned short type;
  unsigned short level_meter_type;
  unsigned long long int mclk;
  unsigned short max_mv;
  unsigned char max_attenuator;
} DdsConfig;

typedef struct __attribute__((packed))
{
  unsigned int  mclk;
  unsigned char channels;
  unsigned char bits;
  unsigned char dds_clock;
} PWMConfig;

extern Device *device_list[MAX_DEVICES];
extern void *device_data[MAX_DEVICES];
extern void *device_config[MAX_DEVICES];

void BuildDeviceList(void);
void InitDeviceLists(void);
int BuildMeterConfig(void *buffer, const MeterConfig *config, const char *name);
int BuildPowerMeterConfig(void *buffer, const PowerMeterConfig *config, const char *name);
int BuildPWMConfig(void *buffer, const PWMConfig *config, const char *name);

#endif
