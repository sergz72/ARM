#ifndef _93CXX_16_H
#define _93CXX_16_H

void _93CXX_16_write_enable(int channel);
int _93CXX_16_write(int channel, unsigned int address, unsigned int data, int timeout);
void _93CXX_16_read(int channel, unsigned int address, unsigned short *data);
int _93CXX_16_erase(int channel, unsigned int address, int timeout);

#endif
