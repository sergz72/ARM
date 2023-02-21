#ifndef _DEV_CAP_METER_H
#define _DEV_CAP_METER_H

void cap_meter_init(void);
void cap_meter_on(void);
void cap_meter_off(void);

void* cap_meter_initializer(void);
void cap_meter_ui_init_handler(void* config);
void cap_meter_ui_handler(void* data, void* config);
int cap_meter_ui_keyboard_handler(void *config, unsigned int event);

#endif
