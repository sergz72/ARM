#include <24c02.h>
#include <i2c_soft.h>
#include "board.h"

unsigned int _24C02_write(unsigned int device_address, unsigned char word_address, unsigned char *data, unsigned int data_length)
{
  return i2c_soft_command(device_address, &word_address, 1, data, data_length, NULL, 0);
}

unsigned int _24C02_read(unsigned int device_address, unsigned char word_address, unsigned char *buffer, unsigned int bytes_count)
{
  return i2c_soft_command(device_address, &word_address, 1, NULL, 0, buffer, bytes_count);
}

