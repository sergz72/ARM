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

unsigned int cursorLine, cursorPosition1, cursorPosition2, cursorEnabled, displayIsOff;

static unsigned char display_data[MAX7219_NUM_PARALLEL][MAX7219_NUM_SERIES * 8];
static unsigned char display_rawdata[MAX7219_NUM_PARALLEL][MAX7219_NUM_SERIES * 8];

static int display_on_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem display_on_command_items[] = {
    {NULL, NULL, display_on_handler}
};
static const ShellCommand display_on_command = {
    display_on_command_items,
    "display_on",
    NULL
};

static int display_off_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem display_off_command_items[] = {
    {NULL, NULL, display_off_handler}
};
static const ShellCommand display_off_command = {
    display_off_command_items,
    "display_off",
    NULL
};

void UI_Init(void)
{
  max7219_init(15);
  cursorEnabled = displayIsOff = 0;
}

void LED_ClearScreen(void)
{
  memset(display_data, 0, sizeof(display_data));
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

void LED_Write_String(int line, unsigned int dots, const char *buffer)
{
  unsigned int d, i;

  d = 1;
  for (i = 0; i < 8 * MAX7219_NUM_SERIES; i++)
  {
    display_data[line][i] = buffer[i];
    if (dots & d)
      display_data[line][i] |= 0x80;
    d <<= 1;
  }
}

void LED_Printf(int line, unsigned int dots, const char *format, ...)
{
  char buffer[8 * MAX7219_NUM_SERIES + 1];
  va_list vArgs;

  va_start(vArgs, format);
  vsnprintf(buffer, sizeof(buffer), format, vArgs);
  va_end(vArgs);
  LED_Write_String(line, dots, buffer);
}

int Process_Timer_Event(int data_ready, unsigned int keyboard_status)
{
  int i, update;
  ui_data_handler_type handler;

  update = 0;

  if (keyboard_status)
  {
    i = process_keyboard_event(keyboard_status);
    if (i)
      update = 1;
  }

  if (current_keyboard_device >= 0)
  {
    switch (data_ready)
    {
    case 1:
      if (cursorEnabled && !displayIsOff)
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
  }

  return update;
}

void enableCursor(unsigned int line, unsigned int position1, unsigned int position2)
{
  cursorLine = line;
  cursorPosition1 = position1;
  cursorPosition2 = position2;
  cursorEnabled = 1;
}

void display_on(void)
{
  max7219_on();
  displayIsOff = 0;
}

void display_off(void)
{
  max7219_off();
  displayIsOff = 1;
}

void display_toggle(void)
{
  if (displayIsOff)
    display_on();
  else
    display_off();
}

static int display_on_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  display_on();
  return 0;
}

static int display_off_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  display_off();
  return 0;
}

void BuildUIShellCommands(void)
{
  shell_register_command(&display_on_command);
  shell_register_command(&display_off_command);
}
