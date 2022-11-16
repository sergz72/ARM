#include "board.h"
#include <spi_flash.h>

unsigned char spi_flash_sr_16_bit;
unsigned char spi_flash_chip_erase_command;

int spi_flash_check_id(void)
{
  union {
    unsigned char id[4];
    int code;
  } id;

  spi_flash_wait();

  spi_command(0x15, NULL, id.id, 3, 1);

  id.id[3] = 0;
  id.code >>= 8;

  if (id.code != 0x651F) // 25F512
  {
    //Jedec ID
    spi_command(0x9F, NULL, id.id, 4, 1);
    id.code >>= 8;
    if (id.code != 0x1540EF) // W25Q16
      return 0;
    spi_flash_sr_16_bit = 1;
    spi_flash_chip_erase_command = 0x60;
    return id.code;
  }

  spi_flash_sr_16_bit = 0;
  spi_flash_chip_erase_command = 0x62;

  return id.code;
}

unsigned char spi_flash_rdsr(void)
{
  unsigned char data[2];
  spi_command(5, NULL, data, 2, 1);
  return data[1];
}

unsigned char spi_flash_wait(void)
{
  unsigned char sr;

  do
  {
    sr = spi_flash_rdsr();
  } while (sr & 1);

  return sr;
}

void spi_flash_chip_erase(void)
{
  unsigned char sr[2];
  
  sr[0] = spi_flash_wait();
  sr[1] = 0;

  spi_command(6, NULL, NULL, 1, 1); // WREN
  if (sr[0] & 0x0C) // BP0, BP1
  {
    spi_flash_wait();
    sr[0] = 2;
    if (spi_flash_sr_16_bit)
      spi_command(1, sr, NULL, 3, 1); // WRSR
    else
      spi_command(1, sr, NULL, 2, 1); // WRSR
  }

  spi_flash_wait();
  spi_command(spi_flash_chip_erase_command, NULL, NULL, 1, 1);
}

typedef union {
  unsigned char chars[4];
  int offset;
} address;

void spi_flash_block_erase(unsigned char command, int offset)
{
  address addr;

  spi_flash_wait();
  spi_command(6, NULL, NULL, 1, 1); // WREN
  spi_flash_wait();
  addr.offset = offset;
  addr.chars[3] = addr.chars[0];
  addr.chars[0] = addr.chars[2];
  addr.chars[2] = addr.chars[3];
  spi_command(command, addr.chars, NULL, 4, 1);
}

void spi_flash_write(const char *buffer, int offset, int size)
{
  address addr;
  int count;

  spi_flash_wait();

  while (size)
  {
    addr.offset = offset;
    spi_flash_wait();
    spi_command(6, NULL, NULL, 1, 1); // WREN
    spi_flash_wait();
    addr.chars[3] = addr.chars[0];
    addr.chars[0] = addr.chars[2];
    addr.chars[2] = addr.chars[3];
    spi_command(2, addr.chars, NULL, 4, 0);
    count = offset & 0x1;
    if (!count)
      count = 1;
    if (count > size)
      count = size;
    size -= count;
    offset += count;
    while (count--)
      spi_byte((unsigned char)*buffer++);
    SPI_CS_SET; // set CS
  }
}

void spi_flash_read(char *buffer, int offset, int size)
{
  address addr;

  spi_flash_wait();
  addr.offset = offset;
  spi_flash_wait();
  addr.chars[3] = addr.chars[0];
  addr.chars[0] = addr.chars[2];
  addr.chars[2] = addr.chars[3];
  spi_command(3, addr.chars, NULL, 4, 0);
  while (size--)
    *buffer++ = spi_byte(0);

  SPI_CS_SET; // set CS
}
