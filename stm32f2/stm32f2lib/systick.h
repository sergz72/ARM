#ifndef _SYSTICK_H
#define _SYSTICK_H

#define STK_CTRL_CLKSOURCE_LSB		2U
#define STK_CTRL_CLKSOURCE_AHB_DIV8	0U
#define STK_CTRL_CLKSOURCE_AHB		1U

void systick_set_reload(unsigned int value);
void systick_set_value(unsigned int value);
unsigned int systick_get_value(void);
void systick_set_clocksource(unsigned int clocksource);
void systick_interrupt_enable(void);
void systick_interrupt_disable(void);
void systick_counter_enable(void);
void systick_counter_disable(void);
int systick_get_countflag(void);

#endif
