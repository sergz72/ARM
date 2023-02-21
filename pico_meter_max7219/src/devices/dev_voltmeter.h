#ifndef _DEV_COLTMETER_H
#define _DEV_VOLTMETER_H

void* voltmeter_initializer(void);
void voltmeter_ui_init_handler(void* config);
void voltmeter_ui_handler(void* data, void* config);
void* voltmeter_data_collector(int step, void* config, void* prev_data);
void adc_start_conversion(int channel);
int adc_read(void);

#endif
