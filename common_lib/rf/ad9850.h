#ifndef _AD9850_H_
#define _AD9850_H_

#define AD9850_ACCUMULATOR_BITS 32
#define AD9850_PHASE_BITS 5

void ad9850_power_down(int channel);
void ad9850_power_up(int channel);
void ad9850_set_freq_word(int channel, unsigned long word, unsigned char phase);
void ad9850_init(void);
void ad9850_reset(int channel);

#endif
