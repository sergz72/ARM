#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define CAPTURE_PIN_BASE 0
#define CAPTURE_PIN_COUNT 2
#define CAPTURE_PIO pio0
#define CAPTURE_SM 0


void SystemInit(void);
void LogicAnalyserStart(void);
void LogicAnalyserStop(void);

#endif
