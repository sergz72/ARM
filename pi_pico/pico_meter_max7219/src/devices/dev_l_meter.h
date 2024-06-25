#ifndef PICO_METER_MAX7219_DEV_L_METER_H
#define PICO_METER_MAX7219_DEV_L_METER_H

void* l_meter_initializer(void);
void l_meter_ui_init_handler(void* config);
void l_meter_ui_handler(void* data, void* config);
int l_meter_ui_keyboard_handler(void *config, unsigned int event);

#endif //PICO_METER_MAX7219_DEV_L_METER_H
