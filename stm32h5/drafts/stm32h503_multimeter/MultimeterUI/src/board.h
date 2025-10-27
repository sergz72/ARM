#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#include "multimeter_enums.h"
#endif

#define LCD_RST_SET
#define DRAW_TEXT_MAX 20

#define ZOOM 4

#define TIMER_DELAY 100

#define CAPACITANCE_COEFFICIENT_100K 1962851UL
#define CAPACITANCE_COEFFICIENT_1K 19628UL
#define CAPACITANCE_OFFSET_1K 23
#define CAPACITANCE_OFFSET_100K 23
#define CAPACITANCE_MIN_VALUE_1K 200000

#define KB_MODE 1
#define KB_BUTTON1 2
#define KB_BUTTON2 3
#define KB_BUTTON3 4
#define KB_MODE_LONG 5
#define KB_BUTTON1_LONG 6
#define KB_BUTTON2_LONG 7
#define KB_BUTTON3_LONG 8

#define USE_MYVSPRINTF
#define LCD_PRINTF_BUFFER_LENGTH 20

#define MEASUREMENT_UNITS_COUNT 3

#define AD7793_MAX_CHANNELS 1
#define AD7793_RDY_GET(channel) 0

#define ADS1220_MAX_CHANNELS 1
#define ADS1220_DRDY_GET(channel) 0

#define AD7793_SET_CHANNEL_CURRENT_SOURCE_CALLBACK ad7793_change_current_source_callback
#define ADS1220_SET_CHANNEL_CURRENT_SOURCE_CALLBACK ads1220_change_current_source_callback

extern volatile int capacitance_measurement_done;

#include "multimeter_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

void charge_off(void);
void charge_on(int channel);
void discharge_off(void);
void discharge_on(void);
void capacitance_measurement_start(int channel);
unsigned int get_capacitance_measurement_start_time(void);
unsigned int get_capacitance_measurement_end_time(void);
void delayms(unsigned int ms);
void delay(unsigned int us);
int get_lcd_buffer_bit(int x, int y);
unsigned int get_keyboard_status(void);
void power_off(void);
unsigned int calculate_inductance(unsigned long long int frequency);
void ad7793_change_current_source_callback(int channel, CurrentSourceLevel current_level);
void ads1220_change_current_source_callback(int channel, CurrentSourceLevel current_level);

#ifdef __cplusplus
}
#endif

#include <lcd_sh1107.h>

#endif
