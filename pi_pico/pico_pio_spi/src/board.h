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
#define PIN_NCS  26
#define PIN_SCK  27
#define PIN_MOSI 28
#define PIN_MISO 29

#define I2C_SDA_PIN 14
#define I2C_SCL_PIN 15
#define I2C_INST    i2c1
#define I2C_TIMEOUT 1000000 // 1s

void SystemInit(void);
int spi_set_baudrate(unsigned int);
void spi_transfer(const unsigned char *data, unsigned int num_bytes, unsigned char *rdata);

#endif
