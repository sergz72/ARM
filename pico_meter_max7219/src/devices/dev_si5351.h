#ifndef STM32F401_METER_DEV_SI5351_H
#define STM32F401_METER_DEV_SI5351_H

#define SI5351_DEVICE_ID 0xC0

void* si5351_initializer(void);
void si5351_ui_init_handler(void* config);
void si5351_ui_keyboard_handler(void *config, unsigned int event);

#endif //STM32F401_METER_DEV_SI5351_H
