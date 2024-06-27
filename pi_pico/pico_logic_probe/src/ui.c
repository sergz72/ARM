#include "board.h"
#include "logic_analyser/logic_analyser.h"
#include "ui.h"

volatile int requestToStop;
volatile int doNotResetCPU1;

/*static void DrawChar(unsigned int col, unsigned int y, char c, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int swapColors)
{
  char text[2];
  text[0] = c;
  text[1] = 0;
  if (swapColors)
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, bkColor, textColor, NULL);
  else
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}

static void DrawText(unsigned int col, unsigned int y, char *text, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor)
{
  LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}*/

void UI_Init(void)
{
  LcdInit();
  LALcdInit();
  requestToStop = doNotResetCPU1 = 0;
}

void SafeLcdUpdate(void)
{
  if (requestToStop)
  {
    while (true)
      tight_loop_contents();
  }
  doNotResetCPU1 = 1;
  LcdUpdate();
  doNotResetCPU1 = 0;
}