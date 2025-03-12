#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define PIN_LED 25

// for frequency meter - 1s interval
#define PIN_GATE 10
//module reset
#define PIN_RESET 11

//module 1
#define PIN_SDA1 2
#define PIN_SCL1 3
#define PIN_1_0  4
#define PIN_1_1  5
#define PIN_1_2  0
#define PIN_1_3  1

//module 2
#define PIN_SDA2 6
#define PIN_SCL2 7
#define PIN_2_0  8
#define PIN_2_1  9
#define PIN_2_2  -1
#define PIN_2_3  -1

//module 3
#define PIN_SDA3 12
#define PIN_SCL3 13
#define PIN_3_0  14
#define PIN_3_1  15
#define PIN_3_2  -1
#define PIN_3_3  -1

// module 4
#define PIN_SDA4 17
#define PIN_SCL4 16
#define PIN_4_0  19
#define PIN_4_1  18
#define PIN_4_2  -1
#define PIN_4_3  -1

// module 5
#define PIN_SDA5 26
#define PIN_SCL5 22
#define PIN_5_0  28
#define PIN_5_1  27
#define PIN_5_2  21
#define PIN_5_3  20

#define MAX_DEVICES 5

#define INTERNAL_DEVICES \
,{\
    .device_id = INTERNAL_PWM_DEVICE_ID,\
    .public_id = PUBLIC_ID_PWM,\
    .initializer = internal_pwm_initializer,\
    .timer_event = NULL,\
    .message_processor = pwm_message_processor,\
    .save_config = internal_pwm_save_config,\
    .is_allowed_for_module_id = AlwaysAllowed\
},\
{\
    .device_id = INTERNAL_FREQUENCY_METER_DEVICE_ID,\
    .public_id = PUBLIC_ID_METER,\
    .initializer = internal_frequency_meter_initializer,\
    .timer_event = internal_frequency_meter_timer_event,\
    .message_processor = NULL,\
    .save_config = internal_frequency_meter_save_config,\
    .is_allowed_for_module_id = AlwaysAllowed\
}

#define INTERNAL_DEVICES_COUNT 2

int counter_get(int module_id, int pin_id);
void counter_pio_init(int module_id, unsigned int sm);
int alloc_pio(int module_id);
int pwm_enable(int module_id, int pin_id, int enable);
int pwm_set_frequency_and_duty(int module_id, int pin_id, unsigned int frequency, unsigned int duty);

#include <hardware/gpio.h>
#include "core_main.h"
#include "dev_internal_frequency_meter.h"
#include "dev_internal_pwm.h"

#endif
