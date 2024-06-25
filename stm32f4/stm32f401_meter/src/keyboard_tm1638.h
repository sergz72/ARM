#ifndef STM32F401_METER_KEYBOARD_TM1638_H
#define STM32F401_METER_KEYBOARD_TM1638_H

#define MAX_KEYS 8
#define MAX_SYMBOLS 8
#define MAX_LEDS 8

int keyboard_init(void);
void keyboard_set_leds(unsigned int led_state);
void keyboard_set_display(unsigned char *symbols);
unsigned int keyboard_get_status(void);

#endif //STM32F401_METER_KEYBOARD_TM1638_H
