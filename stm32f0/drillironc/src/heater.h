#ifndef _HEATER_H
#define _HEATER_H

void heater_on(unsigned int);
void heater_off(unsigned int);
int heater_handler(char c);
void heater_timer_proc(void);
void heater_init(void);
void heater_send_status(void);

#endif
