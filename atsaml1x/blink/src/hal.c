#include "board.h"

/*
 * PA00(1) = LED
 */
static void ports_init(void)
{
  LED_TIMER_OFF;
  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA00;
}

void SysInit(void)
{
  ports_init();
}

void _init(void)
{
}
