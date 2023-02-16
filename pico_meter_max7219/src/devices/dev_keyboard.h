#ifndef STM32F401_METER_DEV_KEYBOARD_H
#define STM32F401_METER_DEV_KEYBOARD_H

#define KEYBOARD_EVENT_KEY0  0
#define KEYBOARD_EVENT_KEY1  1
#define KEYBOARD_EVENT_KEY2  2
#define KEYBOARD_EVENT_KEY3  3
#define KEYBOARD_EVENT_KEY4  4
#define KEYBOARD_EVENT_KEY5  5
#define KEYBOARD_EVENT_KEY6  6
#define KEYBOARD_EVENT_KEY7  7
#define KEYBOARD_EVENT_KEY8  8
#define KEYBOARD_EVENT_KEY9  9
#define KEYBOARD_EVENT_LEAVE 10
#define KEYBOARD_EVENT_ENTER 11
#define KEYBOARD_EVENT_F1    12
#define KEYBOARD_EVENT_F2    13
#define KEYBOARD_EVENT_F3    14
#define KEYBOARD_EVENT_F4    15

void build_keyboard_device_list(int keyboard_present);
int process_keyboard_event(unsigned int keyboard_status);
unsigned int keyboard_get_filtered_status(void);
void process_cursor_off_event(void);

#endif //STM32F401_METER_DEV_KEYBOARD_H
