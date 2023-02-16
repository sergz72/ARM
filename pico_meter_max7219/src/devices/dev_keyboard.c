#include "dev_keyboard.h"
#include <devices.h>
#include "board.h"
#include <seven_seg.h>
#include <memory.h>
#include "ui.h"

static unsigned int current_keyboard_status;
static int current_keyboard_device;

static void show_menu(void)
{
}

static void send_keyboard_event(unsigned int event)
{
  device_list[current_keyboard_device]->ui_keyboard_handler(device_config[current_keyboard_device], event);
}

unsigned int convert_keyboard_status(unsigned int keyboard_status)
{
  unsigned int v = 1, key = KEYBOARD_EVENT_KEY1;
  int i;

  for (i = 0; i < 8; i++)
  {
    if (keyboard_status & v)
      return key;
    v <<= 1;
    key++;
  }

  return 0;
}

void process_cursor_off_event(void)
{
  if (current_keyboard_device >= 0)
    send_keyboard_event(KEYBOARD_EVENT_LEAVE);
}

int process_keyboard_event(unsigned int keyboard_status)
{
  if (current_keyboard_device >= 0)
  {
    if (keyboard_status == KEYBOARD_EVENT_LEAVE)
    {
      cursorEnabled = 0;
      send_keyboard_event(KEYBOARD_EVENT_LEAVE);
      current_keyboard_device = -1;
      show_menu();
    }
    else
      send_keyboard_event(keyboard_status);
    return 1;
  }
  else
  {
    if (keyboard_status < found_devices)
    {
      current_keyboard_device = keyboard_status;
      show_menu();
      send_keyboard_event(KEYBOARD_EVENT_ENTER);
      return 1;
    }
  }
  return 0;
}

unsigned int keyboard_get_filtered_status(void)
{
  unsigned int status = 0;//keyboard_get_status();
  if (status != current_keyboard_status)
  {
    current_keyboard_status = status;
    return status;
  }
  return 0;
}
