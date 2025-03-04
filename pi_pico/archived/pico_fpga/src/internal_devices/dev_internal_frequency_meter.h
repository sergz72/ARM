#ifndef DEV_INTERNAL_FREQUENCY_METER_H
#define DEV_INTERNAL_FREQUENCY_METER_H

#define INTERNAL_FREQUENCY_METER_DEVICE_ID 0x101

void* internal_frequency_meter_initializer(int idx, void **data);
int internal_frequency_meter_timer_event(int idx, int step, void* config, void *data, int interrupt, unsigned char *buffer);
int internal_frequency_meter_save_config(int idx, void *buffer);

#endif
