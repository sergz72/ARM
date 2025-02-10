#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <samd11c14a.h>

#define LED_TIMER_ON PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA24
#define LED_TIMER_OFF PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA24

#define INTERRUPT_FLAG_SET PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA25
#define INTERRUPT_FLAG_CLR PORT_REGS->GROUP[0].PORT_OUTCLR = PORT_PA25

#define I2C_ADDRESS 2

#define I2C_SOFT
#define I2C_TIMEOUT 1000000
#define i2c_dly delay(5)

/*
 *PA04(14) -> SDA
 *PA02(13) -> SCL
 */
#define SDA_PIN PORT_PA04
#define SCL_PIN PORT_PA02
#define SCL_HIGH(channel) PORT_REGS->GROUP[0].PORT_DIRCLR = SCL_PIN
#define SCL_LOW(channel) PORT_REGS->GROUP[0].PORT_DIRSET = SCL_PIN
#define SDA_HIGH(channel) PORT_REGS->GROUP[0].PORT_DIRCLR = SDA_PIN
#define SDA_LOW(channel) PORT_REGS->GROUP[0].PORT_DIRSET = SDA_PIN
#define SDA_IN(channel) (PORT_REGS->GROUP[0].PORT_IN & SDA_PIN)
#define SCL_IN(channel) (PORT_REGS->GROUP[0].PORT_IN & SCL_PIN)

#define SYSTICK_MULTIPLIER 48

/*
 *PA08(2) = button
 */
#define BUTTON_PIN PORT_PA08
#define BUTTON_PRESSED PORT_REGS->GROUP[0].PORT_IN & BUTTON_PIN

extern volatile int interrupt_request;
extern const void *txbufs[2];

void SysInit(void);
void delay(unsigned int us);
void delayms(unsigned int ms);

#endif
