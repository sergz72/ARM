#ifndef UI_H
#define UI_H

#include <ws2812_spi.h>

void UI_Init(void);
void Process_Timer_Event(void);
void UI_CommonInit(void);
void DrawMode(void);
void Process_Button_Events(void);
void calculate_led_data_without_rs(void);
void calculate_led_data_with_rs(void);

extern int voltmeter_mode;
extern ws2812_rgb led_data[WS2812_MAX_LEDS];
extern unsigned int uh_changed_to, ul_changed_to;

#endif
