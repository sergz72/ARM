#include "board.h"
#include "ui.h"
#include <pico/stdlib.h>
#include "keyboard.h"

int main()
{
  SystemInit();
  stdio_init_all();

  UI_Init();
  KeyboardInit();

  while (true)
  {
    sleep_ms(TIMER_INTERVAL);
    Process_Timer_Event();
  }
}
