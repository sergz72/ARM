#ifndef _TTP229_H_
#define _TTP229_H_

void ttp229_interrupt(void);
void ttp229_init(void);
int ttp229_get(void);

void ttp229_semaphore_init(void);
void ttp229_semaphore_release(void);
void ttp229_semaphore_acquire(void);
void ttp229_gpio_init(void);

#endif
