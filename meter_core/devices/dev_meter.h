#ifndef DEV_METER_H
#define DEV_METER_H

#include "devices.h"

void meter_initializer(DeviceObject *o);
int meter_timer_event(DeviceObject *o, int step, unsigned char *buffer);
int meter_save_config(DeviceObject *o, void *buffer);

#endif
