#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <hardware/gpio.h>

#define TIMER_INTERVAL 250

#define PWM_PERIOD (125000000 / 3)

#define INPUT_PIN 18
#define GEN_PIN   16

#define SPI3_CLK_PIN 0
#define SPI3_DIO_PIN 1
#define SPI3_CS_PIN  2

#define SPI3_DIO_SET gpio_set_dir(SPI3_DIO_PIN, false)
#define SPI3_DIO_CLR gpio_set_dir(SPI3_DIO_PIN, true)
#define SPI3_CLK_SET gpio_put(SPI3_CLK_PIN, 1)
#define SPI3_CLK_CLR gpio_put(SPI3_CLK_PIN, 0)
#define SPI3_CHECK_DIO gpio_get(SPI3_DIO_PIN)
#define SPI3_CS_SET gpio_put(SPI3_CS_PIN, 1)
#define SPI3_CS_CLR gpio_put(SPI3_CS_PIN, 0)
#define SPI3_CUSTOM_DELAY
#define SPI3_LSB_FIRST

#define CAP_IN_PIN    28
#define CAP_RLOW_PIN  26
#define CAP_RHIGH_PIN 27

#define CAP_MAX 60000000
#define CAP_OFFSET 121

#define KOEF_MUL 10000077
#define KOEF_DIV 10000000

extern volatile unsigned int counter_value, cap_value, cap_value_updated;

void SystemInit(void);

#endif
