#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define MAX_SHELL_COMMANDS                 100
#define MAX_SHELL_COMMAND_PARAMETERS       10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE                 20
#define SHELL_HISTORY_ITEM_LENGTH          100

#define SPI_PIO  pio0
#define SPI_SM   0
#define PIN_SCK  15
#define PIN_MOSI 26
#define PIN_MISO 27
#define PIN_NCS  14

void SystemInit(void);
int spi_set_baudrate(unsigned int);
void spi_transfer(const unsigned char *data, unsigned int num_bytes, unsigned char *rdata);

#endif
