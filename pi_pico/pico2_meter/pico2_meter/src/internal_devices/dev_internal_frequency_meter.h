#ifndef DEV_INTERNAL_FREQUENCY_METER_H
#define DEV_INTERNAL_FREQUENCY_METER_H

struct _DeviceObject;

#define INTERNAL_FREQUENCY_METER_DEVICE_ID 0x101

void internal_frequency_meter_initializer(struct _DeviceObject *o);
int internal_frequency_meter_timer_event(struct _DeviceObject *o, int step, unsigned char *buffer);
int internal_frequency_meter_save_config(struct _DeviceObject *o, void *buffer);

#endif
