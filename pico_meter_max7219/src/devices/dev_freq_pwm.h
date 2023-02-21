#ifndef _DEV_FREQ_METER_H_
#define _DEV_FREQ_METER_H_ 

void freq_pwm_on(void);
void freq_pwm_off(void);
void pwm_on(int channel);
void pwm_off(int channel);
void pwm_set_freq(int channel, unsigned int freq);
void pwm_set_duty(int channel, unsigned int duty);

void *freq_pwm_initializer(void);
void freq_pwm_ui_init_handler(void* config);
void freq_pwm_ui_handler(void* data, void* config);
int freq_pwm_ui_keyboard_handler(void *config, unsigned int event);

#endif
