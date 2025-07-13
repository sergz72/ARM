#ifndef _BOARD_COMMON_H
#define _BOARD_COMMON_H

#ifndef NULL
#define NULL 0
#endif

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200

#define DEFAULT_DAC1_VOLTAGE  400
#define DEFAULT_DAC3_VOLTAGE  2400

#define TIMER_EVENT_FREQUENCY 10

#define USART_INTERRUPT_PRIORITY 2
#define COUNTERS_INTERRUPT_PRIORITY 1
#define TIMER_INTERRUPT_PRIORITY 0

extern unsigned int counter_low, counter_high, counter_freq_low, counter_freq_high;
extern volatile unsigned int counter_low_counter, counter_high_counter, freq_low_counter, freq_high_counter;
extern volatile int timer_event;

#ifdef __cplusplus
extern "C" {
#endif

unsigned int mv_to_12(unsigned int mv);
unsigned int mv_from_12(unsigned int code);
unsigned int uv_from_12(unsigned int code);
void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty);
void stop_counters(void);
void start_counters(void);
void set_l_voltage(unsigned int value);
void set_h_voltage(unsigned int value);
void set_dac_voltage(unsigned int value);
unsigned int get_dac_voltage(void);
unsigned int get_l_voltage(void);
unsigned int get_h_voltage(void);
void adc_start(void);
unsigned int get_adc_voltage(void);
void connect_pullup(void);
void disconnect_pullup(void);
void CustomMainInit(void);
void PeriodicTimerStart(void);
int getch_(void);
void update_counters(void);

#ifdef __cplusplus
}
#endif

#define LCD_PRINTF_BUFFER_LENGTH 30
#define DRAW_TEXT_MAX 20
#define USE_MYVSPRINTF

#define WS2812_MAX_LEDS 4
#define WS2812_MAX_VALUE 0x40

#endif
