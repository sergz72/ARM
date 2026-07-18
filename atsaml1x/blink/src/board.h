#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <saml10e16a.h>

#define LED_TIMER_ON PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA00
#define LED_TIMER_OFF PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA00

#define SYSTICK_MULTIPLIER 4

void SysInit(void);

#endif
