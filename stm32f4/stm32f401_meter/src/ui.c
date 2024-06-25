#include "board.h"
#include "ui.h"
#include "devices.h"
#include "dev_keyboard.h"
#include <lcd.h>
#include <malloc.h>

#define CURSOR_TIMEOUT 50

unsigned int cursorPosition, cursorEnabled;

#define ROWS 2

void UI_Init(void)
{
  int i, id, prev_id, idx, x, y, max_devices;
  void* * prev_config;

  LcdInit();
  LcdOn();

  cursorEnabled = 0;

  prev_config = NULL;
  prev_id = idx = max_devices = 0;
  for (i = 0; i < MAX_DEVICES; i++)
  {
    id = device_list[i];
    if (id)
    {
      x = GetX(i);
      y = GetY(i);
      if (id != prev_id || !max_devices)
      {
        idx = 0;
        prev_id = id;
        prev_config = device_config[i];
      }
      max_devices = CallUIInitHandler(id, x, y, idx++, prev_config);
    }
  }
}

int GetX(int idx)
{
  return (idx % ROWS) * LCD_WIDTH / ROWS;
}

int GetY(int idx)
{
  return (idx / ROWS) * LCD_HEIGHT / ROWS;
}

void Process_Timer_Event(int data_ready, unsigned int keyboard_status)
{
  int i, id, prev_id, idx, x, y, max_devices;
  void* prev_data, *prev_config;

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
    if (!data_ready && (i || x))
      LcdUpdate();
  }

  if (data_ready)
  {
    prev_data = prev_config = NULL;
    prev_id = idx = max_devices = 0;
    for (i = 0; i < MAX_DEVICES; i++)
    {
      id = device_list[i];
      if (id)
      {
        x = GetX(i);
        y = GetY(i);
        if (id != prev_id || !max_devices)
        {
          idx = 0;
          prev_id = id;
          prev_data = device_data[i];
          prev_config = device_config[i];
        }
        max_devices = CallUIHandler(id, x, y, idx++, prev_data, prev_config);
      }
    }
    for (i = 0; i < MAX_DEVICES; i++)
    {
      if (device_list[i])
        free(device_data[i]);
    }

    LcdUpdate();
  }
}

void enableCursor(unsigned int position)
{
  cursorPosition = position;
  cursorEnabled = CURSOR_TIMEOUT;
}

void DrawLcdChar(int x, int col, int y, char c, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int swapColors)
{
  char text[2];
  text[0] = c;
  text[1] = 0;
  if (swapColors)
    LcdDrawText(x + col * (f->character_max_width + f->character_spacing), y, text, f, bkColor, textColor, NULL);
  else
    LcdDrawText(x + col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}

void DrawLcdText(int x, int col, int y, char* text, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int swapColors)
{
  if (swapColors)
    LcdDrawText(x + col * (f->character_max_width + f->character_spacing), y, text, f, bkColor, textColor, NULL);
  else
    LcdDrawText(x + col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}
