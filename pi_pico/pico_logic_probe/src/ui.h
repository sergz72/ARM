#ifndef _UI_H
#define _UI_H

#define IDNOEVENT 0
#define IDBUTTONLEFT 1
#define IDBUTTONRIGHT 2
#define IDBUTTONTOP 3
#define IDBUTTONBOTTOM 4
#define IDBUTTONCENTER 5
#define IDBUTTONA 6
#define IDBUTTONB 7

extern volatile int requestToStop;
extern volatile int doNotResetCPU1;

void UI_Init(void);
void SafeLcdUpdate(void);

#endif
