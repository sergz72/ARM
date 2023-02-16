#include "board.h"
#include "ui.h"
#include "devices.h"
#include "dev_keyboard.h"
#include <malloc.h>
#include <max7219.h>

#define CURSOR_TIMEOUT 50

unsigned int cursorPosition, cursorEnabled;

void UI_Init(void)
{
  int i;

  max7219_init(1);

  cursorEnabled = 0;

  for (i = 0; i < found_devices; i++)
    device_list[i]->ui_init_handler(device_config[i]);
}

void Process_Timer_Event(int data_ready, unsigned int keyboard_status)
{
  int i, x;

  x = 0;
  if (cursorEnabled)
  {
    cursorEnabled--;
    if (!cursorEnabled)
    {
      process_cursor_off_event();
      x = 1;
    }
  }

  if (keyboard_status || x)
  {
    i = 0;
    if (keyboard_status)
      i = process_keyboard_event(keyboard_status);
    //if (!data_ready && (i || x))
    //  max();
  }

  if (data_ready)
  {
    for (i = 0; i < found_devices; i++)
    {
      device_list[i]->ui_data_handler(device_data[i], device_config[i]);
      free(device_data[i]);
    }
    //LcdUpdate();
  }
}

void enableCursor(unsigned int position)
{
  cursorPosition = position;
  cursorEnabled = CURSOR_TIMEOUT;
}
