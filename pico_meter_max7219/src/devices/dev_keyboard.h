#ifndef STM32F401_METER_DEV_KEYBOARD_H
#define STM32F401_METER_DEV_KEYBOARD_H

#define KEYBOARD_EVENT_KEY0  1
#define KEYBOARD_EVENT_KEY1  2
#define KEYBOARD_EVENT_KEY2  3
#define KEYBOARD_EVENT_KEY3  4
#define KEYBOARD_EVENT_KEY4  5
#define KEYBOARD_EVENT_KEY5  6
#define KEYBOARD_EVENT_KEY6  7
#define KEYBOARD_EVENT_KEY7  8
#define KEYBOARD_EVENT_KEY8  9
#define KEYBOARD_EVENT_KEY9  10
#define KEYBOARD_EVENT_ENTER 11
#define KEYBOARD_EVENT_LEAVE 12
#define KEYBOARD_EVENT_F1    13
#define KEYBOARD_EVENT_F2    14
#define KEYBOARD_EVENT_F3    15
#define KEYBOARD_EVENT_F4    16

int process_keyboard_event(unsigned int keyboard_status);
unsigned int keyboard_get_filtered_status(void);
void process_cursor_off_event(void);
void dev_keyboard_init(void);
int process_current_keyboard_device_switch(void);

extern int current_keyboard_device;

#endif //STM32F401_METER_DEV_KEYBOARD_H
