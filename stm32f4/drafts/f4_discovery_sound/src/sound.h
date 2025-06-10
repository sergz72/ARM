#ifndef SOUND_H
#define SOUND_H

#define SAMPLE_RATE 48000

#define SOUND_OUT_BUFFER_SIZE 1024

extern unsigned int sound_out_buffer[];

void sound_init(void);
void dac_set_frequency(unsigned int frequency);
void dac_set_volume(unsigned short volume);

#endif