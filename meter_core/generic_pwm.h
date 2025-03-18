#ifndef _GENERIC_PWM
#define _GENERIC_PWM

#define PWM_COMMAND_SET_PERIOD_AND_DUTY    2
#define PWM_COMMAND_ENABLE_OUTPUT          5

#define PWM_DEVICE_ID 4

#define DEVICE_COMMAND_PWM_COMMAND 3

typedef struct __attribute__((packed))
{
  unsigned int  mclk;
  unsigned char channels;
  unsigned char bits;
  unsigned char prescaler_bits;
  unsigned char dds_clock;
} PWMConfig;

typedef struct __attribute__((__packed__)) {
  unsigned char parameter;
} pwm_cmd1;

typedef struct __attribute__((__packed__)) {
  unsigned int period;
  unsigned int duty;
  unsigned short prescaler;
} pwm_cmd10;


typedef struct __attribute__((__packed__)) {
  unsigned char device_command;
  unsigned char command;
  unsigned char channel;
  union
  {
    pwm_cmd1 c1;
    pwm_cmd10 c10;
  };
} pwm_i2c_command;

#endif
