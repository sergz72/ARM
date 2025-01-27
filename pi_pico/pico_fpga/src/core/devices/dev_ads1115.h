#ifndef _DEV_ADS1115_H
#define _DEV_ADS1115_H

#define ADS1115_DEVICE_ID 0x90

void* ads1115_initializer(int idx, void **data);
int ads1115_timer_event(int idx, int step, void* config, void *data, int interrupt, unsigned char *buffer);
int ads1115_save_config(void *buffer);

#endif
