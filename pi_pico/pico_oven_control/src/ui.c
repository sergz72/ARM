#include "board.h"
#include "ui.h"
#include <fonts/font36.h>
#include <fonts/font18.h>

static void ShowTemperature(int temperature)
{
  LcdPrintf("%3d", 0, 0, &courierNew36ptFontInfo, 1, temperature);
}

static void ShowProgramStep(void)
{
  const ProgramItem *step = get_current_step();
  if (step == NULL)
    LcdDrawText(0, courierNew36ptFontInfo.char_height, "   |   ", &courierNew18ptFontInfo, WHITE_COLOR, BLACK_COLOR, NULL);
  else
    LcdPrintf("%3d|%3d", 0, courierNew36ptFontInfo.char_height, &courierNew18ptFontInfo, 1, step->temperature, get_current_step_time());
}

void UI_Init(void)
{
  LcdInit();
  ShowProgramStep();
  LcdUpdate();
}

void Process_Timer_Event(unsigned int keyboard_status, int temperature, enum OvenStatus status)
{
  ShowTemperature(temperature);
  switch (keyboard_status)
  {
    case KB_START:
      start_program();
      break;
    case KB_STOP:
      stop_program();
      break;
    default:
      break;
  }
  ShowProgramStep();
  LcdUpdate();
}
