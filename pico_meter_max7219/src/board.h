#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <hardware/gpio.h>

#define FREQ1_PIN 10
#define FREQ2_PIN 9

#define PWM1_PIN  15
#define PWM2_PIN  14

#define CAP_IN_PIN    11
#define CAP_RHIGH_PIN 12
#define CAP_RLOW_PIN  13

#define SPI_4BIT_DIO0_PIN 18
#define SPI_4BIT_DIO1_PIN 19
#define SPI_4BIT_DIO2_PIN 20
#define SPI_4BIT_DIO3_PIN 21
#define SPI_4BIT_CLK_PIN  16
#define SPI_4BIT_CS_PIN   17

#define KB1_PIN 22
#define KB2_PIN 2
#define KB3_PIN 3
#define KB4_PIN 4

#define SCL_PIN 1
#define SDA_PIN 0

#define SPI_4BIT_CLK_CLR gpio_put(SPI_4BIT_CLK_PIN, 0)
#define SPI_4BIT_CLK_SET gpio_put(SPI_4BIT_CLK_PIN, 1)
#define SPI_4BIT_CS_CLR gpio_put(SPI_4BIT_CS_PIN, 0)
#define SPI_4BIT_CS_SET gpio_put(SPI_4BIT_CS_PIN, 1)

#define MAX7219_NUM_PARALLEL 4
#define MAX7219_NUM_SERIES 1

#define CAP_MAX 60000000
#define CAP_OFFSET 121

#define KOEF_MUL 10000077
#define KOEF_DIV 10000000

#define SI5351_XTAL_FREQ 25000000
#define SI5351_CHANNELS 1

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define I2C_TIMEOUT 1000000

#define KBD_COUNTX 4

extern volatile unsigned int counter_value1, counter_value2, cap_value, cap_value_updated;

void SystemInit(void);
void SPI_4BIT_DATA_SET(unsigned char data);
unsigned char HALKbdHandler(void);

#endif
