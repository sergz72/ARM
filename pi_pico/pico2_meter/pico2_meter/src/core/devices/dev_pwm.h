#ifndef DEV_PWM_H
#define DEV_PWM_H

#include <devices.h>

typedef struct {
    int (*set_frequency_and_duty)(int idx, void *cfg, void *data, int channel, unsigned int frequency, unsigned int duty);
    int (*enable_output)(int idx, void *cfg, void *data, int channel, int enable);
    void *cfg;
} dev_pwm;

void pwm_initializer(void);
int pwm_message_processor(DeviceObject *o, unsigned char *buffer, int len);

#endif
