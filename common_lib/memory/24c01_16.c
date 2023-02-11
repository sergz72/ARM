#include <24c01_16.h>
#include <i2c_soft.h>
#include "board.h"

int _24C01_16_write(int channel, unsigned int device_address, unsigned char word_address, unsigned char *data,
                    unsigned int data_length, int timeout)
{
  return i2c_soft_command(channel, device_address, &word_address, 1, data,
                          data_length, NULL, 0, timeout);
}

int _24C01_16_read(int channel, unsigned int device_address, unsigned char word_address, unsigned char *buffer,
                   unsigned int bytes_count, int timeout)
{
  return i2c_soft_command(channel, device_address, &word_address, 1, NULL,
                          0, buffer, bytes_count, timeout);
}

