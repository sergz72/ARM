#include "board.h"
#include "ui.h"
#include "devices.h"
#include "dev_keyboard.h"
#include <malloc.h>
#include <max7219.h>
#include <stdarg.h>
#include <memory.h>
#include <seven_seg.h>
#include <stdio.h>

#define CURSOR_TIMEOUT 50

unsigned int cursorLine, cursorPosition1, cursorPosition2, cursorEnabled;

static unsigned char display_data[MAX7219_NUM_PARALLEL][MAX7219_NUM_SERIES * 8];
static unsigned char display_rawdata[MAX7219_NUM_PARALLEL][MAX7219_NUM_SERIES * 8];

void UI_Init(void)
{
  max7219_init(1);
  cursorEnabled = 0;
}

void LED_Update(void)
{
  int i, j;
  unsigned char c;

  for (i = 0; i < MAX7219_NUM_PARALLEL; i++)
  {
    for (j = 0; j < MAX7219_NUM_SERIES * 8; j++)
    {
      c = display_data[i][j];
      display_rawdata[i][j] = seven_seg_get_symbol(c & 0x7F) | (c & 0x80);
    }
  }

  max7219_out(&display_rawdata[0][0]);
}

void LED_Write_String(int line, const char *buffer)
{
  memcpy(display_data[line], buffer, 8 * MAX7219_NUM_SERIES);
}

void LED_Printf(int line, const char *format, ...)
{
  char buffer[8 * MAX7219_NUM_SERIES + 1];
  va_list vArgs;

  va_start(vArgs, format);
  vsnprintf(buffer, sizeof(buffer), format, vArgs);
  va_end(vArgs);
  LED_Write_String(line, buffer);
}

int Process_Timer_Event(int data_ready, unsigned int keyboard_status)
{
  int i, update;
  ui_data_handler_type handler;

  update = 0;
  if (cursorEnabled)
  {
    cursorEnabled--;
    if (!cursorEnabled)
    {
      process_cursor_off_event();
      update = 1;
    }
  }

  if (keyboard_status)
  {
    i = process_keyboard_event(keyboard_status);
    if (i)
      update = 1;
  }

  switch (data_ready)
  {
    case 1:
      if (cursorEnabled)
      {
        for (i = (int)cursorPosition1; i <= cursorPosition2; i++)
          display_rawdata[cursorLine][i] = 0;
        max7219_out(&display_rawdata[0][0]);
      }
      break;
    case 2:
      if (cursorEnabled)
        update = 1;
      handler = device_list[current_keyboard_device]->ui_data_handler;
      if (handler)
      {
        handler(device_data, device_config[current_keyboard_device]);
        update = 1;
      }
      if (device_data)
      {
        free(device_data);
        device_data = NULL;
      }
      break;
  }

  return update;
}

void enableCursor(unsigned int line, unsigned int position1, unsigned int position2)
{
  cursorLine = line;
  cursorPosition1 = position1;
  cursorPosition2 = position2;
  cursorEnabled = CURSOR_TIMEOUT;
}
