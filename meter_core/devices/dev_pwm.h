#ifndef DEV_PWM_H
#define DEV_PWM_H

#include <devices.h>

typedef struct {
    int (*set_frequency_and_duty)(DeviceObject *o, int channel, unsigned short prescaler, unsigned int frequency,
                                  unsigned int duty);
    int (*enable_output)(DeviceObject *o, int channel, int enable);
    PWMConfig cfg;
} dev_pwm;

void pwm_initializer(void);
int pwm_message_processor(DeviceObject *o, unsigned char *buffer, int len);
int pwm_save_config(DeviceObject *o, void *buffer);
void external_pwm_initializer(DeviceObject *o);

#endif
