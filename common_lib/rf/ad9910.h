#ifndef _AD9910_H_
#define _AD9910_H_

#define AD9910_ACCUMULATOR_BITS 32

typedef struct
{
  unsigned int refclk;
  unsigned int pllclk;
  unsigned int charge_pump_current_ua;
} AD9910_Config;

void ad9910_power_down(int channel);
void ad9910_power_up(int channel);
void ad9910_set_freq_phase_amplitude(int channel, unsigned long freq_word, unsigned int phase, unsigned int amplitude);
int ad9910_init(int channel, const AD9910_Config *config);
void ad9910_write(int channel, unsigned char command, const unsigned char *data, unsigned int length); // should be defined in hal.c

#endif
