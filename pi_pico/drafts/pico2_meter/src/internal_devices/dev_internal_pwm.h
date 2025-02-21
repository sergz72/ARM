#ifndef DEV_INTERNAL_PWM_H
#define DEV_INTERNAL_PWM_H

struct _DeviceObject;

#define INTERNAL_PWM_DEVICE_ID 0x100

void internal_pwm_initializer(struct _DeviceObject *o);
int internal_pwm_save_config(struct _DeviceObject *o, void *buffer);

#endif
