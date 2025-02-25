#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define LED_GPIO GPIOA
#define LED_PIN DL_GPIO_PIN_0
#define LED_IOMUX IOMUX_PINCM1

#define SYSTICK_MULTIPLIER 32

void delayms(unsigned int ms);
void delay(unsigned int us);
void SystemInit(void);

#endif
