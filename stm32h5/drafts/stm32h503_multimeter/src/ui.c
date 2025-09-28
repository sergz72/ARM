#include "board.h"
#include "ui.h"
#include <lcd_sh1107.h>
#include <fonts/font10.h>

extern unsigned char sh1107_buffer[];

#define MAIN_FONT courierNew10ptFontInfo

static void DrawMenu(void)
{
  LcdDrawText(0, MAIN_FONT.char_height*9, " Z | ? | ? ", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, NULL);
}

void UI_Init(void)
{
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  int y = 0;

  LcdDrawText(0, y, "00.0C 0.00V", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);

  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1U 0.", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1I  0.", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1P 0.", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);

  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2U 0.", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2I  0.", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2P 0.", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);

  DrawMenu();

  LcdUpdate();
}

void Process_Timer_Event(void)
{
  //todo
}
