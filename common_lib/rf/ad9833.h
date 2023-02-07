#ifndef _AD9833_H_
#define _AD9833_H_

#define AD9833_CFG_RESET    0x100
#define AD9833_CFG_SLEEP1   0x80
#define AD9833_CFG_SLEEP12  0x40
#define AD9833_CFG_SINUS    0
#define AD9833_CFG_TRIANGLE 2
#define AD9833_CFG_MSB      0x28
#define AD9833_CFG_MSBDIV2  0x20
#define AD9833_CFG_FSELECT  0x800
#define AD9833_CFG_PSELECT  0x400

#define AD9833_ACCUMULATOR_BITS 28

void ad9833_write(int channel, unsigned short data);

void ad9833_set_config(int channel, unsigned short cfg);
void ad9833_set_freq0_word(int channel, unsigned long word);
void ad9833_set_freq1_word(int channel, unsigned long word);
void ad9833_set_phase0(int channel, unsigned short phase);
void ad9833_set_phase1(int channel, unsigned short phase);
int ad9833_is_output_enabled(unsigned short cfg);
void ad9833_update_mode(int channel, unsigned short *cfg, unsigned short new_mode);

#endif
