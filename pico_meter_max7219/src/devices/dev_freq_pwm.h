#ifndef _DEV_FREQ_METER_H_
#define _DEV_FREQ_METER_H_ 

#include <shell.h>

void *freq_pwm_initializer(void);
void *freq_pwm_data_collector(int step, void* config, void *data);
void freq_pwm_ui_handler(void* data, void* config);
int freq_pwm_print_config(printf_func pfunc, void *device_config);
int freq_pwm_set_config(printf_func pfunc, int argc, char **argv, void *device_config);

#endif
