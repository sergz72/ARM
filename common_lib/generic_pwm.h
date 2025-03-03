#ifndef _GENERIC_PWM
#define _GENERIC_PWM

typedef struct __attribute__((packed))
{
  unsigned int  mclk;
  unsigned char channels;
  unsigned char bits;
  unsigned char dds_clock;
} PWMConfig;

#endif
