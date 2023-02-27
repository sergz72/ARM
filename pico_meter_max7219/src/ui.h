#ifndef _UI_H
#define _UI_H

int Process_Timer_Event(int data_ready, unsigned int keyboard_status);
void UI_Init(void);
void enableCursor(unsigned int line, unsigned int position1, unsigned int position2);
void LED_Write_String(int line, unsigned int dots, const char *buffer);
void LED_Printf(int line, unsigned int dots, const char *format, ...);
void LED_Update(void);
void LED_ClearScreen(void);
void BuildUIShellCommands(void);
void display_on(void);
void display_off(void);
void display_toggle(void);

extern unsigned int cursorPosition1, cursorPosition2, cursorLine, cursorEnabled, displayIsOff;

#endif
