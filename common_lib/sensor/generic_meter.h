#ifndef _GENERIC_METER
#define _GENERIC_METER

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

#endif
