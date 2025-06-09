#ifndef SOUND_H
#define SOUND_H

#define SAMPLE_RATE 16000

#define SOUND_OUT_BUFFER_SIZE 1024

extern unsigned int sound_out_buffer[];

int sound_init(void);

#endif