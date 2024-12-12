#ifndef _93CXX_16_H
#define _93CXX_16_H

int _93CXX_16_write_enable(int channel, int timeout);
int _93CXX_16_write(int channel, unsigned int address, unsigned int data, int timeout);
int _93CXX_16_read(int channel, unsigned int address, unsigned short *data, int timeout);

#endif
