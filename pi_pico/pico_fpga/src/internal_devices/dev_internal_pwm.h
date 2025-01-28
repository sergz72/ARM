#ifndef DEV_INTERNAL_PWM_H
#define DEV_INTERNAL_PWM_H

#define INTERNAL_PWM_DEVICE_ID 0x100

void* internal_pwm_initializer(int idx, void **data);
int internal_pwm_save_config(int idx, void *buffer);

#endif
