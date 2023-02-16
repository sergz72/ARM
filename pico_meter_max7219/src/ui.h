#ifndef _UI_H
#define _UI_H

void Process_Timer_Event(int data_ready, unsigned int keyboard_status);
void UI_Init(void);
void enableCursor(unsigned int position);

extern unsigned int cursorPosition, cursorEnabled;

#endif
