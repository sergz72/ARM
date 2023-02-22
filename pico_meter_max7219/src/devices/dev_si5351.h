#ifndef STM32F401_METER_DEV_SI5351_H
#define STM32F401_METER_DEV_SI5351_H

#include <shell.h>

#define SI5351_DEVICE_ID 0xC0

void* si5351_initializer(void);
void si5351_ui_init_handler(void* config);
int si5351_ui_keyboard_handler(void *config, unsigned int event);
int si5351_print_config(printf_func pfunc, void* config);
int si5351_set_config(printf_func pfunc, int argc, char** argv, void* config);

#endif //STM32F401_METER_DEV_SI5351_H
