#include "dev_keyboard.h"
#include <devices.h>
#include "ui.h"
#include <keyboard.h>
//#include <stdio.h>
#include <stddef.h>

static unsigned int current_keyboard_status;
int current_keyboard_device;
static int current_keyboard_device_switch;

static void show_menu(void)
{
  int line, pos, idx;
  unsigned char buffer[8], *p;

  idx = 0;
  for (line = 0; line < 4; line++)
  {
    p = buffer;
    for (pos = 0; pos < 4; pos++)
    {
      if (idx < found_devices)
      {
        *p++ = device_list[idx]->short_name[0];
        *p++ = device_list[idx]->short_name[1] | 0x80;
      }
      else
      {
        *p++ = ' ';
        *p++ = ' ';
      }
      idx++;
    }
    LED_Write_String(line, 0, (const char*)buffer);
  }
}

static int send_keyboard_event(unsigned int event)
{
  ui_keyboard_handler_type handler = device_list[current_keyboard_device]->ui_keyboard_handler;
  if (handler)
    return handler(device_config[current_keyboard_device], event);
  return 0;
}

void dev_keyboard_init(void)
{
  current_keyboard_device = -1;
  current_keyboard_device_switch = -2;
  current_keyboard_status = 0;
  show_menu();
  LED_Update();
}

int process_current_keyboard_device_switch(void)
{
  ui_init_handler_type handler = NULL;

  if (current_keyboard_device_switch != -2)
  {
    current_keyboard_device = current_keyboard_device_switch;
    current_keyboard_device_switch = -2;
    if (current_keyboard_device >= 0)
    {
      handler = device_list[current_keyboard_device]->ui_init_handler;
      if (handler)
        handler(device_config[current_keyboard_device]);
    }
    else
    {
      show_menu();
      return 1;
    }
  }

  return handler != NULL;
}

int process_keyboard_event(unsigned int keyboard_status)
{
  //printf("%d %d", current_keyboard_device, keyboard_status);
  if (current_keyboard_device >= 0)
  {
    if (keyboard_status == KEYBOARD_EVENT_LEAVE)
    {
      if (current_keyboard_device_switch == -2)
      {
        if (!send_keyboard_event(KEYBOARD_EVENT_LEAVE))
        {
          cursorEnabled = 0;
          current_keyboard_device_switch = -1;
        }
      }
    }
    else
      send_keyboard_event(keyboard_status);
    return 1;
  }
  else
  {
    if (keyboard_status > KEYBOARD_EVENT_KEY0 && keyboard_status <= found_devices+1 && current_keyboard_device_switch == -2)
    {
      current_keyboard_device_switch = (int)keyboard_status - 2;
      return 0;
    }
  }
  return 0;
}

unsigned int keyboard_get_filtered_status(void)
{
  unsigned int status = KbdCheck();
  if (status != current_keyboard_status)
  {
    //printf("new keyboard status: %d\n", current_keyboard_status);
    current_keyboard_status = status;
    return status;
  }
  return 0;
}
