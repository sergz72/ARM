#ifndef _SPI_MEMORY_H
#define _SPI_MEMORY_H

#define SPI_MEMORY_DEFAULT_WRITE_COMMAND 2
#define SPI_MEMORY_DEFAULT_READ_COMMAND 3
#define SPI_MEMORY_DEFAULT_FAST_READ_COMMAND 0x0B
#define SPI_MEMORY_DEFAULT_READ_ID_COMMAND 0x9F
#define SPI_MEMORY_DEFAULT_RESET_ENABLE_COMMAND 0x66
#define SPI_MEMORY_DEFAULT_RESET_COMMAND 0x99

void spi_trfr(int channel, int nwrite, const unsigned char *wdata, int nop_cycles, int nread, unsigned char *rdata, int set_cs); // should be defined in hal.c
void spi_finish(int channel); // should be defined in hal.c

unsigned int spi_memory_read_id(int channel, int address_length, unsigned char command);
void spi_memory_wren(int channel, unsigned char command);
void spi_memory_write(int channel, unsigned char command, unsigned int address, int address_length,
                        const unsigned char *buffer, int size);
void spi_memory_read(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char *buffer, int size, int nop_cycles);

void spi_memory_write_cb(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char (*next_byte)(void), int size);
int spi_memory_read_cb(int channel, unsigned char command, unsigned int address, int address_length,
                    int (*set_byte)(unsigned char), int size, int nop_cycles);

void psram_reset(int channel);
unsigned int psram_read_id(int channel);
void psram_read(int channel, unsigned int address, unsigned char *buffer, int count);
void psram_fast_read(int channel, unsigned int address, unsigned char *buffer, int count);
void psram_write(int channel, unsigned int address, unsigned char *buffer, int count);
int psram_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
int psram_fast_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
void psram_write_cb(int channel, unsigned int address, unsigned char (*next_byte)(void), int count);

#endif
