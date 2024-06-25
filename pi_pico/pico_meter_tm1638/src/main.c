#include "board.h"
#include "ui.h"
#include <pico/stdlib.h>
#include <stdio.h>
#include "cap_meter.h"

int main()
{
  SystemInit();
  stdio_init_all();

  UI_Init();

  while (true)
  {
    sleep_ms(TIMER_INTERVAL);
    Process_Timer_Event();
  }
}
