#include "board.h"
#include "ui.h"
#include <lcd_sh1107.h>
#include <fonts/font10.h>

extern unsigned char sh1107_buffer[];

#define MAIN_FONT courierNew10ptFontInfo

void UI_Init(void)
{
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  LcdDrawText(0, 0, "Test1234567", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdDrawText(0, 20, "1234567Test", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdUpdate();
}

void Process_Timer_Event(void)
{
  //todo
}
