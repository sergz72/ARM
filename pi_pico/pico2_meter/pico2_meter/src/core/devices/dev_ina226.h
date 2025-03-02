#ifndef _DEV_INA226_H_
#define _DEV_INA226_H_

#include <devices.h>

typedef struct {
  int voltage, current;
} DEV_INA226Data;

typedef struct {
  int r;
} DEV_INA226Config;

#define INA226_DEVICE_ID 0x80

void ina226_initializer(DeviceObject *o);
int ina226_timer_event(DeviceObject *o, int step, int interrupt, unsigned char *buffer);
int ina226_save_config(DeviceObject *o, void *buffer);

#endif

