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

#define MODE_PWM      0
#define MODE_SINE     1
#define MODE_SQUARE   2
#define MODE_TRIANGLE 3
#define MODE_MAX      3

void Process_Timer_Event(void);
void UI_Init(void);

#endif
