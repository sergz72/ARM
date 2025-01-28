#ifndef _DEV_INA3221_H_
#define _DEV_INA3221_H_

#define INA3221_DEVICE_ID 0x82

void* ina3221_initializer(int idx, void **data);
int ina3221_timer_event(int idx, int step, void* config, void *data, int interrupt, unsigned char *buffer);
int ina3221_save_config(int idx, void *buffer);

#endif

