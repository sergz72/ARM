#ifndef HAL_H
#define HAL_H

#include "board.h"

extern volatile unsigned int command_ready, timer_interrupt;
extern const void *txbufs[];
extern unsigned char rxbuf[MAX_TRANSFER_SIZE];
extern unsigned char status;

void SysInit(void *rxaddress, const void *txaddress);
void handler_init(void);
unsigned short adc_get(void);
void set_frequency_code(int channel, unsigned long long int code, unsigned short divider);
void set_mode(int channel, int mode);
void enable_output(int channel, int enable);
void timer_enable(void);
void status_updated(void);

#endif
