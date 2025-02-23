#ifndef HAL_H
#define HAL_H

#include "board.h"
#include <generic_dds.h>

#define DDS_COMMAND_QUEUE_LENGTH 5

extern volatile unsigned int command_ready, timer_interrupt;
extern const void *txbufs[];
extern volatile dds_i2c_command commands[DDS_COMMAND_QUEUE_LENGTH];
extern unsigned char status;
extern volatile int command_to_process;

void SysInit(const void *txaddress);
void handler_init(void);
unsigned short adc_get(void);
void set_frequency_code(int channel, unsigned long long int code, unsigned short divider);
void set_mode(int channel, int mode);
void enable_output(int channel, int enable);
void timer_enable(void);
void status_updated(void);

#endif
