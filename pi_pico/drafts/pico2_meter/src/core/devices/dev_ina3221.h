#ifndef _DEV_INA3221_H_
#define _DEV_INA3221_H_

#include <devices.h>

#define INA3221_DEVICE_ID 0x82

void ina3221_initializer(DeviceObject *o);
int ina3221_timer_event(DeviceObject *o, int step, int interrupt, unsigned char *buffer);
int ina3221_save_config(DeviceObject *o, void *buffer);

#endif

