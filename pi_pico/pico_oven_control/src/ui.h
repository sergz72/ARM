#ifndef _UI_H
#define _UI_H

#include "controller.h"

void UI_Init(void);
void Process_Timer_Event(unsigned int keyboard_status, int temperature, enum OvenStatus status);

#endif
