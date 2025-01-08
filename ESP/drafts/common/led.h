//
// Created by sergi on 04.09.2022.
//

#ifndef TEMP_HUMI_PRES_SENSOR_LED_H
#define TEMP_HUMI_PRES_SENSOR_LED_H

void set_led(uint32_t red, uint32_t green, uint32_t blue);
void set_led_red(void);
void set_led_green(void);
void set_led_blue(void);
void set_led_yellow(void);
void set_led_purple(void);
void set_led_white(void);
void led_off(void);
void configure_led(void);

#endif //TEMP_HUMI_PRES_SENSOR_LED_H
