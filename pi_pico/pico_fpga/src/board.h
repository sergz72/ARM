#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define PIN_LED 25

// for logging
#define UART_BAUD_RATE 115200
#define UART_PORT 0
#define PIN_UART_TXD 16
#define PIN_UART_RXD 17

// i2c to FPGA
#define PIN_I2C_SDA_OE  14
#define PIN_I2C_SCL_OE  15
#define PIN_I2C_SDA_IN  12
#define PIN_I2C_SCL_IN  13
//FPGA device id selector
#define PIN_DEVICE_ID0  3
#define PIN_DEVICE_ID1  4
#define PIN_DEVICE_ID2  5
//FPGA module reset
#define PIN_RESET       22
// 2 wire spi to FPGA
#define PIN_SCLK        10
#define PIN_SNCS        11
#define PIN_SDI0        8
#define PIN_SDI1        9
#define PIN_SDO0        6
#define PIN_SDO1        7
// interrupt from FPGA
#define PIN_INTERRUPT   2
// for frequency meter - 1s interval
#define PIN_GATE        1

//module 4 - internal
#define PIN_SDA4 27
#define PIN_SCL4 26
#define PIN_4_0  28

// module 5 - internal
#define PIN_SDA5 19
#define PIN_SCL5 18
#define PIN_5_0  20
#define PIN_5_1  21

#define SPI_2BIT_CLK_CLR gpio_put(PIN_SCLK, false)
#define SPI_2BIT_CLK_SET gpio_put(PIN_SCLK, true)
#define SPI_2BIT_CS_CLR gpio_put(PIN_SNCS, false)
#define SPI_2BIT_CS_SET gpio_put(PIN_SNCS, true)

void SPI_2BIT_DATA_SET(unsigned int data);
unsigned int spi2_data_get(void);
#define SPI_2BIT_DATA_GET spi2_data_get()

#define MAX_DEVICES 6

#define INTERNAL_DEVICES \
,{\
.name = "Internal PWM",\
.device_id = INTERNAL_PWM_DEVICE_ID,\
.public_id = PUBLIC_ID_PWM,\
.initializer = internal_pwm_initializer,\
.timer_event = NULL,\
.message_processor = pwm_message_processor,\
.save_config = internal_pwm_save_config\
},\
{\
    .name = "Internal Frequency meter",\
    .device_id = INTERNAL_FREQUENCY_METER_DEVICE_ID,\
    .public_id = PUBLIC_ID_METER,\
    .initializer = internal_frequency_meter_initializer,\
    .timer_event = internal_frequency_meter_timer_event,\
    .message_processor = NULL,\
    .save_config = internal_frequency_meter_save_config\
}

#define COUNTER4_0_SM 0
#define COUNTER5_0_SM 1
#define COUNTER5_1_SM 2

#define INTERNAL_DEVICES_COUNT 2

int counter_get(int module_id, int pin_id);
void counter_enable(int module_id);
void counter_pio_init(unsigned int sm, unsigned int pin);
int pwm_enable(int module_id, int pin_id, int enable);
int pwm_set_frequency_and_duty(int module_id, int pin_id, unsigned int frequency, unsigned int duty);

#include <hardware/gpio.h>
#include "core_main.h"
#include "dev_internal_frequency_meter.h"
#include "dev_internal_pwm.h"

#endif
