#ifndef _DEV_COLTMETER_H
#define _DEV_VOLTMETER_H

#include <shell.h>

void* voltmeter_initializer(void);
void voltmeter_ui_init_handler(void* config);
void voltmeter_ui_handler(void* data, void* config);
void* voltmeter_data_collector(int step, void* config, void* prev_data);
int voltmeter_calibrate(printf_func pfunc, int argc, char** argv, void* device_config);

void adc_start_conversion(int channel);
int adc_get_result(void);
void adc_wait(void);

#endif
