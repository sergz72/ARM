#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <hardware/gpio.h>

#define FALSE 0
#define TRUE  1

#define MAX_SHELL_COMMANDS 100
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define WS2812_PIN 16

#define ADS1259_MAX_CHANNELS 1
#define ADS1259_RESET_PIN 28
#define ADS1259_START_PIN 27
#define ADS1259_DRDY_PIN  26

#define SPI1_TX_PIN  15
#define SPI1_SCK_PIN 14
#define SPI1_CS_PIN  13
#define SPI1_RX_PIN  12

#define ADS1259_RESET_SET(channel) gpio_put(ADS1259_RESET_PIN, 1)
#define ADS1259_RESET_CLR(channel) gpio_put(ADS1259_RESET_PIN, 0)
#define ADS1259_START_SET(channel) gpio_put(ADS1259_START_PIN, 1)
#define ADS1259_START_CLR(channel) gpio_put(ADS1259_START_PIN, 0)
#define ADS1259_DRDY_GET(channel) gpio_get(ADS1259_DRDY_PIN)

#define ADS1259_TIMEOUT 1000

#define ADS1259_INPUT_COEFFICIENT 0.180327869
#define ADS1259_VREF 2.5

#define AD7124_MAX_CHANNELS 1
#define AD7124_TIMEOUT 1000
#define AD7124_INPUT_COEFFICIENT 1.0
#define AD7124_VREF 2.5
#define AD7124_RDY_GET(channel) gpio_get(SPI1_RX_PIN)

void SystemInit(void);
void WS2812Init(void);
void put_pixel(unsigned int pixel_grb);
unsigned int urgb_u32(unsigned int r, unsigned int g, unsigned int b);
void delayms(unsigned int ms);
void set_cpha1(void);
void set_cpol1cpha1(void);

#endif
