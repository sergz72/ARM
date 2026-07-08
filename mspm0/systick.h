#ifndef _SYSTICK_H
#define _SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

void systick_start(unsigned int us);
void systick_stop(void);
void systick_wait(void);
void delay(unsigned int us);
void delayms(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif
