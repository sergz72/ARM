#ifndef _SPI_MEMORY_H
#define _SPI_MEMORY_H

#define SPI_MEMORY_DEFAULT_WRITE_COMMAND 2
#define SPI_MEMORY_DEFAULT_READ_COMMAND 3
#define SPI_MEMORY_DEFAULT_FAST_READ_COMMAND 0x0B
#define SPI_MEMORY_DEFAULT_READ_ID_COMMAND 0x9F

unsigned int spi_memory_read_id(int channel, int address_length, unsigned char command);
void spi_memory_wren(int channel, unsigned char command);
void spi_memory_write(int channel, unsigned char command, unsigned int address, int address_length,
                        const unsigned char *buffer, int size);
void spi_memory_read(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char *buffer, int size, int skip);

void spi_memory_write_cb(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char (*next_byte)(void), int size);
int spi_memory_read_cb(int channel, unsigned char command, unsigned int address, int address_length,
                    int (*set_byte)(unsigned char), int size, int skip);

unsigned int psram_read_id(int channel);
int psram_read(int channel, unsigned int address, unsigned char *buffer, int count);
int psram_fast_read(int channel, unsigned int address, unsigned char *buffer, int count);
int psram_write(int channel, unsigned int address, unsigned char *buffer, int count);
int psram_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
int psram_fast_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
int psram_write_cb(int channel, unsigned int address, unsigned char (*next_byte)(void), int count);

#endif
