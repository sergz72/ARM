#ifndef _AD9850_H_
#define _AD9850_H_

#define AD9850_ACCUMULATOR_BITS 32
#define AD9850_PHASE_BITS 5

void ad9850_power_down(int channel);
void ad9850_power_up(int channel);
void ad9850_set_freq_word(int channel, unsigned long word, unsigned char phase);
void ad9850_init(int channel, unsigned char refclk_multiplier_enable, unsigned char parallel_load);
void ad9850_reset(int channel);
void ad9850_write(int channel, const unsigned char *data, unsigned int length); // can be redefined in hal.c

#endif
