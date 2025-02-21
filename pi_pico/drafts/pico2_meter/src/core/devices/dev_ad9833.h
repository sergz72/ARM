#ifndef _DEV_AD9833_H
#define _DEV_AD9833_H

#define AD9833_DEVICE_ID 2

#include <devices.h>

void ad9833_initializer(DeviceObject *o);
int ad9833_save_config(DeviceObject *o, void *buffer);

#endif
