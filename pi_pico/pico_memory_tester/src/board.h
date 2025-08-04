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

#define QSPI_CS_PIN 26
#define QSPI_SI_SIO0_PIN 27
#define QSPI_SO_SIO1_PIN 15
#define QSPI_SIO2_PIN 14
#define QSPI_SIO3_PIN 29
#define QSPI_CLK_PIN 28

#define WS2812_PIN 16

void qspi_trfr(int nwrite, const unsigned char *wdata, int nop_cycles, int nread, unsigned char *rdata, int set_cs);

void SystemInit(void);
void WS2812Init(void);
void put_pixel(unsigned int pixel_grb);
unsigned int urgb_u32(unsigned int r, unsigned int g, unsigned int b);

#endif
