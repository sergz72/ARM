#ifndef _TM1638_H_
#define _TM1638_H_

#define TM1638_RED_LED 0x02 // Model 3
#define TM1638_GREEN_LED 0x01 // Model 3
#define TM1638_OFF_LED 0x00

void tm1638_init(void);
void tm1638_set_led(unsigned char position, unsigned char state);
unsigned int tm1638_in(int num_bytes);
void tm1638_out(int pos, int length, unsigned char *data);
void tm1638_brightness(unsigned char brightness);
void tm1638_display_on(void);
void tm1638_display_off(void);
unsigned int tm1638_key_state(void);
void tm1638_set_leds(unsigned char state);

#endif
