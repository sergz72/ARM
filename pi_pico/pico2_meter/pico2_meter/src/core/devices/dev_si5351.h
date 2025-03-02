#ifndef _DEV_SI5351_H
#define _DEV_SI5351_H

#include <devices.h>

#define SI5351_DEVICE_ID 0xC0

void si5351_initializer(DeviceObject *o);
int si5351_save_config(DeviceObject *o, void *buffer);

#endif
