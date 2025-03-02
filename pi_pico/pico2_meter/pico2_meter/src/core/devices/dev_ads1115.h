#ifndef _DEV_ADS1115_H
#define _DEV_ADS1115_H

#define ADS1115_DEVICE_ID 0x90

#include <devices.h>

void ads1115_initializer(DeviceObject *o);
int ads1115_timer_event(DeviceObject *o, int step, int interrupt, unsigned char *buffer);
int ads1115_save_config(DeviceObject *o, void *buffer);

#endif
