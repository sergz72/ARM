#include "board.h"
#include <spi_memory.h>

typedef union {
  unsigned char chars[4];
  unsigned int value;
} cuint;

unsigned int spi_memory_read_id(int channel, int address_length, unsigned char command)
{
  unsigned char data[++address_length];

  data[0] = command;
  for (int i = 1; i < address_length; i++)
    data[i] = 0;

  spi_trfr(channel, address_length, data, 0, 2, data, 1);

  return data[0] | (data[1] << 8);
}

void spi_memory_wren(int channel, unsigned char command)
{
    spi_trfr(channel, 1, &command, 0, 0, NULL, 1); // WREN
}

void address_swap(unsigned char *chars, int address_length)
{
  char c0 = chars[0];
  switch (address_length)
  {
    case 2:
      chars[0] = chars[1];
      chars[1] = c0;
      break;
    case 3:
      chars[0] = chars[2];
      chars[2] = c0;
      break;
    default:
      chars[0] = chars[3];
      chars[3] = c0;
      c0 = chars[2];
      chars[2] = chars[1];
      chars[1] = c0;
      break;
  }
}

void spi_memory_write(int channel, unsigned char command, unsigned int address, int address_length,
                      const unsigned char *buffer, int size)
{
  cuint addr;
  unsigned char data[address_length+1];

  data[0] = command;
  addr.value = address;
  address_swap(addr.chars, address_length);
  for (int i = 1; i <= address_length; i++)
    data[i] = addr.chars[i-1];

  spi_trfr(channel, address_length+1, data, 0, 0, NULL, 0);
  spi_trfr(channel, size, buffer, 0, 0, NULL, 1);
}

void spi_memory_read(int channel, unsigned char command, unsigned int address, int address_length, unsigned char *buffer,
                      int size, int nop_cycles)
{
  cuint addr;
  unsigned char data[address_length+1];

  data[0] = command;
  addr.value = address;
  address_swap(addr.chars, address_length);
  for (int i = 1; i <= address_length; i++)
    data[i] = addr.chars[i-1];
  spi_trfr(channel, address_length+1, data, nop_cycles, size, buffer, 1);
}

void spi_memory_write_cb(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char (*next_byte)(void), int size)
{
  cuint addr;
  unsigned char c;
  unsigned char data[address_length+1];

  data[0] = command;
  addr.value = address;
  address_swap(addr.chars, address_length);
  for (int i = 1; i <= address_length; i++)
    data[i] = addr.chars[i-1];
  spi_trfr(channel, address_length+1, data, 0, 0, NULL, 0);
  while (size--)
  {
    c = next_byte();
    spi_trfr(channel, 1, &c, 0, 0, NULL, 0);
  }
  spi_finish(channel); // set CS
}

int spi_memory_read_cb(int channel, unsigned char command, unsigned int address, int address_length,
                    int (*set_byte)(unsigned char), int size, int nop_cycles)
{
  cuint addr;
  unsigned char c;
  int rc = 0;
  unsigned char data[address_length+1];

  data[0] = command;
  addr.value = address;
  address_swap(addr.chars, address_length);
  for (int i = 1; i <= address_length; i++)
    data[i] = addr.chars[i-1];
  spi_trfr(channel, address_length+1, data, nop_cycles, 0, NULL, 0);
  while (size--)
  {
    spi_trfr(channel, 0, NULL, 0, 1, &c, 0);
    rc = set_byte(c);
    if (rc)
      break;
  }
  spi_finish(channel); // set CS

  return rc;
}

void psram_reset(int channel)
{
  unsigned char data;
  data = SPI_MEMORY_DEFAULT_RESET_ENABLE_COMMAND;
  spi_trfr(channel, 1, &data, 0, 0, NULL, 1); // RESET ENABLE
  data = SPI_MEMORY_DEFAULT_RESET_COMMAND;
  spi_trfr(channel, 1, &data, 0, 0, NULL, 1); // RESET
}

unsigned int psram_read_id(int channel)
{
  return spi_memory_read_id(channel, 3, SPI_MEMORY_DEFAULT_READ_ID_COMMAND);
}

void psram_read(int channel, unsigned int address, unsigned char *buffer, int count)
{
  spi_memory_read(channel, SPI_MEMORY_DEFAULT_READ_COMMAND, address, 3, buffer, count, 0);
}

void psram_fast_read(int channel, unsigned int address, unsigned char *buffer, int count)
{
  spi_memory_read(channel, SPI_MEMORY_DEFAULT_FAST_READ_COMMAND, address, 3, buffer, count, 8);
}

void psram_write(int channel, unsigned int address, unsigned char *buffer, int count)
{
  spi_memory_write(channel, SPI_MEMORY_DEFAULT_WRITE_COMMAND, address, 3, buffer, count);
}

int psram_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count)
{
  return spi_memory_read_cb(channel, SPI_MEMORY_DEFAULT_READ_COMMAND, address, 3, set_byte, count, 0);
}

int psram_fast_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count)
{
  return spi_memory_read_cb(channel, SPI_MEMORY_DEFAULT_FAST_READ_COMMAND, address, 3, set_byte, count, 6);
}

void psram_write_cb(int channel, unsigned int address, unsigned char (*next_byte)(void), int count)
{
  spi_memory_write_cb(channel, SPI_MEMORY_DEFAULT_WRITE_COMMAND, address, 3, next_byte, count);
}
