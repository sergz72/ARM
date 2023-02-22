#ifndef _DEV_FREQ_METER_H_
#define _DEV_FREQ_METER_H_ 

#include <shell.h>

void freq_pwm_on(void);
void freq_pwm_off(void);
void pwm_on(int channel);
void pwm_off(int channel);
void pwm_set_freq(int channel, unsigned int freq);
void pwm_set_duty(int channel, unsigned int duty);
void freq_counter_set_interval(unsigned int interval);
unsigned int get_counter_value1(void);
unsigned int get_counter_value2(void);

void *freq_pwm_initializer(void);
void freq_pwm_ui_init_handler(void* config);
void freq_pwm_ui_handler(void* data, void* config);
int freq_pwm_ui_keyboard_handler(void *config, unsigned int event);
int freq_pwm_calibrate(printf_func pfunc, int argc, char** argv, void* device_config);

#endif
