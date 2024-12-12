#include "board.h"
#include <spi_memory.h>

typedef union {
  unsigned char chars[4];
  unsigned int value;
} cuint;

unsigned int spi_memory_read_id(int channel, int address_length, unsigned char command)
{
  unsigned char data[address_length + 2];

  spi_command(channel, command, NULL, data, address_length + 2, 1);

  return data[address_length] | (data[address_length + 1] << 8);
}

void spi_memory_wren(int channel, unsigned char command)
{
    spi_command(channel, command, NULL, NULL, 0, 1); // WREN
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

  addr.value = address;
  address_swap(addr.chars, address_length);
  spi_command(channel, command, addr.chars, NULL, address_length, 0);
  spi_write(channel, buffer, size);
  spi_finish(channel); // set CS
}

void spi_memory_read(int channel, unsigned char command, unsigned int address, int address_length, unsigned char *buffer,
                      int size, int skip)
{
  cuint addr;
  unsigned char skip_buffer[skip];

  addr.value = address;
  address_swap(addr.chars, address_length);
  spi_command(channel, command, addr.chars, NULL, address_length, 0);
  spi_read(channel, skip_buffer, skip);
  spi_read(channel, buffer, size);

  spi_finish(channel); // set CS
}

void spi_memory_write_cb(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char (*next_byte)(void), int size)
{
  cuint addr;
  unsigned char c;

  addr.value = address;
  address_swap(addr.chars, address_length);
  spi_command(channel, command, addr.chars, NULL, address_length, 0);
  while (size--)
  {
    c = next_byte();
    spi_write(channel, &c, 1);
  }
  spi_finish(channel); // set CS
}

int spi_memory_read_cb(int channel, unsigned char command, unsigned int address, int address_length,
                    int (*set_byte)(unsigned char), int size, int skip)
{
  cuint addr;
  unsigned char skip_buffer[skip], c;
  int rc = 0;

  addr.value = address;
  address_swap(addr.chars, address_length);
  spi_command(channel, command, addr.chars, NULL, address_length, 0);
  spi_read(channel, skip_buffer, skip);
  while (size--)
  {
    spi_read(channel, &c, 1);
    rc = set_byte(c);
    if (rc)
      break;
  }

  spi_finish(channel); // set CS

  return rc;
}

unsigned int psram_read_id(int channel)
{
  return spi_memory_read_id(channel, 3, SPI_MEMORY_DEFAULT_READ_ID_COMMAND);
}

int psram_read(int channel, unsigned int address, unsigned char *buffer, int count)
{
  spi_memory_read(channel, SPI_MEMORY_DEFAULT_READ_COMMAND, address, 3, buffer, count, 0);
  return 0;
}

int psram_fast_read(int channel, unsigned int address, unsigned char *buffer, int count)
{
  spi_memory_read(channel, SPI_MEMORY_DEFAULT_FAST_READ_COMMAND, address, 3, buffer, count, 1);
  return 0;
}

int psram_write(int channel, unsigned int address, unsigned char *buffer, int count)
{
  spi_memory_write(channel, SPI_MEMORY_DEFAULT_FAST_READ_COMMAND, address, 3, buffer, count);
  return 0;
}

int psram_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count)
{
  return spi_memory_read_cb(channel, SPI_MEMORY_DEFAULT_READ_COMMAND, address, 3, set_byte, count, 0);
}

int psram_fast_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count)
{
  return spi_memory_read_cb(channel, SPI_MEMORY_DEFAULT_FAST_READ_COMMAND, address, 3, set_byte, count, 1);
}

int psram_write_cb(int channel, unsigned int address, unsigned char (*next_byte)(void), int count)
{
  spi_memory_write_cb(channel, SPI_MEMORY_DEFAULT_WRITE_COMMAND, address, 3, next_byte, count);
  return 0;
}
