#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define MAX_SHELL_COMMANDS 100
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define SPI_INST spi0
#define SPI_CS_PIN 15
#define SPI_SI_PIN 16
#define SPI_CLK_PIN 18
#define SPI_SO_PIN 19

#define _93CXX_DI_PIN 14
#define _93CXX_DO_PIN 13
#define _93CXX_CLK_PIN 17
#define _93CXX_CS_PIN 20

#define _93CXX_DI_LOW(channel) gpio_put(_93CXX_DI_PIN, 0)
#define _93CXX_DI_HIGH(channel) gpio_put(_93CXX_DI_PIN, 1)
#define _93CXX_CS_LOW(channel) gpio_put(_93CXX_CS_PIN, 0)
#define _93CXX_CS_HIGH(channel) gpio_put(_93CXX_CS_PIN, 1)
#define _93CXX_CLK_LOW(channel) gpio_put(_93CXX_CLK_PIN, 0)
#define _93CXX_CLK_HIGH(channel) gpio_put(_93CXX_CLK_PIN, 1)
#define _93CXX_DO(channel) gpio_get(_93CXX_DO_PIN)
#define _93CXX_DI(channel) gpio_get(_93CXX_DI_PIN)
#define _93CXX_DELAY _93CXX_delay()

void spi_write(int channel, const unsigned char *data, int length);
void spi_read(int channel, unsigned char *data, int length);
void spi_command(int channel, unsigned char cmd, unsigned char *data_in, unsigned char *data_out, int count, int set_cs);
void spi_finish(int channel);

unsigned int spi_get_clock(int channel);
void spi_set_clock(int channel, unsigned int value);
void spi_trfr(int channel, int nwrite, const unsigned char *wdata, int nread, unsigned char *rdata);

void _93CXX_delay(void);

void SystemInit(void);

#include <hardware/gpio.h>

#endif
