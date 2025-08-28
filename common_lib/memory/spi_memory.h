#ifndef _SPI_MEMORY_H
#define _SPI_MEMORY_H

#define SPI_MEMORY_DEFAULT_WRITE_COMMAND 2
#define SPI_MEMORY_DEFAULT_READ_COMMAND 3
#define SPI_MEMORY_DEFAULT_WREN_COMMAND 6
#define SPI_MEMORY_DEFAULT_FAST_READ_COMMAND 0x0B
#define SPI_MEMORY_DEFAULT_READ_ID_COMMAND 0x9F
#define SPI_MEMORY_DEFAULT_RESET_ENABLE_COMMAND 0x66
#define SPI_MEMORY_DEFAULT_RESET_COMMAND 0x99
#define SPI_MEMORY_DEFAULT_ENTER_QSPI_MODE_COMMAND 0x35
#define SPI_MEMORY_DEFAULT_EXIT_QSPI_MODE_COMMAND 0xF5

#define QSPI_MEMORY_DEFAULT_FAST_READ_COMMAND 0xEB
#define QSPI_MEMORY_DEFAULT_WRITE_COMMAND 0x38

#define SPI_MEMORY_DEFAULT_SECTOR_ERASE_COMMAND  0x20
#define SPI_MEMORY_DEFAULT_BLOCK32_ERASE_COMMAND 0x52
#define SPI_MEMORY_DEFAULT_BLOCK64_ERASE_COMMAND 0xD8
#define SPI_MEMORY_DEFAULT_CHIP_ERASE_COMMAND    0xC7

enum spi_memory_erase_command {SECTOR, BLOCK32, BLOCK64, CHIP};

void spi_trfr(int channel, int nwrite, const unsigned char *wdata, int nop_cycles, int nread, unsigned char *rdata, int set_cs); // should be defined in hal.c
void qspi_trfr(int channel, int nwrite, const unsigned char *wdata, int nop_cycles, int nread, unsigned char *rdata, int set_cs); // should be defined in hal.c
void spi_finish(int channel); // should be defined in hal.c
void qspi_set_sio_direction(int out0, int out1, int out2, int out3); // should be defined in hal.c

void spi_memory_init(void);
int spi_memory_read_id(int channel, int address_length, unsigned char command, unsigned int *id);
int spi_memory_wren(int channel, unsigned char command);
void spi_memory_write(int channel, unsigned char command, unsigned int address, int address_length,
                        const unsigned char *buffer, int size);
void spi_memory_read(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char *buffer, int size, int nop_cycles);

void spi_memory_write_cb(int channel, unsigned char command, unsigned int address, int address_length,
                        unsigned char (*next_byte)(void), int size);
int spi_memory_read_cb(int channel, unsigned char command, unsigned int address, int address_length,
                    int (*set_byte)(unsigned char), int size, int nop_cycles);

void psram_reset(int channel);
int psram_read_id(int channel, unsigned int *id);
int psram_read(int channel, unsigned int address, unsigned char *buffer, int count);
int psram_fast_read(int channel, unsigned int address, unsigned char *buffer, int count);
void psram_write(int channel, unsigned int address, unsigned char *buffer, int count);
int psram_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
int psram_fast_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
void psram_write_cb(int channel, unsigned int address, unsigned char (*next_byte)(void), int count);
int psram_enter_qspi_mode(int channel);
int psram_exit_qspi_mode(int channel);

int flash_read_id(int channel, unsigned int *id);
int flash_wren(int channel);
void flash_write_page_cb(int channel, unsigned int address, unsigned char (*next_byte)(void), int count);
int flash_fast_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
int flash_read_cb(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
void flash_write_page(int channel, unsigned int address, unsigned char *buffer, int count);
int flash_fast_read(int channel, unsigned int address, unsigned char *buffer, int count);
int flash_read(int channel, unsigned int address, unsigned char *buffer, int count);
void flash_erase(int channel, enum spi_memory_erase_command command, unsigned int address);
int flash_enter_qspi_mode(int channel);
int flash_exit_qspi_mode(int channel);
void flash_reset(int channel);
void flash_write_cb(int channel, int page_size, unsigned int address, unsigned char (*next_byte)(void), int count);
void flash_write(int channel, int page_size, unsigned int address, unsigned char *buffer, int count);
unsigned char flash_read_sr1(int channel);
unsigned char flash_read_sr2(int channel);

#endif
