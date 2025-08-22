#ifndef HYPERRAM_H
#define HYPERRAM_H

void hyperram_init(int latency);
void hyperram_read(unsigned int address, unsigned char *data, unsigned int nbytes);
void hyperram_write(unsigned int address, unsigned char *data, unsigned int nbytes);
int hyperram_read_callback(unsigned int address, int (*set_byte)(unsigned char c), unsigned int nbytes);
void hyperram_write_callback(unsigned int address, unsigned char (*next_byte)(void), unsigned int nbytes);

#endif
