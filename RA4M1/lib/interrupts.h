#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <bsp_elc.h>

typedef void (* interrupt_handler_t)(void);

unsigned int RegisterApplicationInterrupt(interrupt_handler_t handler, enum e_elc_event_ra4m1 interrupt_type);

#endif
