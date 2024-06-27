#ifndef _LOGIC_ANALYSER_H
#define _LOGIC_ANALYSER_H

extern unsigned int pinState[32][3];

void LAInputsCommand(void (*func)(unsigned int));
void LAStartEvents(void);
void LAStopEvents(void);
void LAInit(void);
void LALcdInit(void);
void LALcdUpdate(void);
void LACPU1Task(void);
int LAGpioCallback(unsigned int gpio, unsigned long events);

#endif
