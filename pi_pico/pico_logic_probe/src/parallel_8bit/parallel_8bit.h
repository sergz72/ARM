#ifndef _PARALLEL_8BIT_H
#define _PARALLEL_8BIT_H

#define P8_RST   DATA8
#define P8_CS    DATA9
#define P8_DC    DATA10
#define P8_WR    DATA11
#define P8_RD    DATA12

extern volatile unsigned int p8code;

void P8Init(void);
void P8StartEvents(void);
void P8StopEvents(void);
void P8LcdInit(void);
void P8LcdUpdate(void);
void P8CPU1Task(void);
int P8GpioCallback(unsigned int gpio, unsigned long events);

#endif
