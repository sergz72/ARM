#ifndef _SPI_FLASH_H
#define _SPI_FLASH_H

#define SPI_FLASH_BLOCK_ERASE_64K 0xD8
#define SPI_FLASH_BLOCK_ERASE_32K 0x52
#define SPI_FLASH_BLOCK_ERASE_4K 0x20

int spi_flash_check_id(void);
unsigned char spi_flash_rdsr(void);
unsigned char spi_flash_wait(void);
void spi_flash_chip_erase(void);
void spi_flash_write(const char *buffer, int offset, int size);
void spi_flash_read(char *buffer, int offset, int size);
void spi_flash_block_erase(unsigned char command, int offset);

#endif
