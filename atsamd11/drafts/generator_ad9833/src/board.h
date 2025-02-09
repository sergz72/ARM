#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define MAX_TRANSFER_SIZE 512

#include <cmsis_compiler.h>

#define WAIT_FOR_INTERRUPT __WFI()

/* SERCOM1 SPI baud value for 4000000 Hz baud rate */
#define SERCOM1_SPIM_BAUD_VALUE 5

#include <samd11c14a.h>

//#define LED_TIMER_ON PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA25
//#define LED_TIMER_OFF PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA25

#define LED_COMMAND_ON PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA25
#define LED_COMMAND_OFF PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA25

#define SPI_MASTER_NSS_PIN PORT_PA28
#define EXTERNAL_RESET_DISABLE

#endif
