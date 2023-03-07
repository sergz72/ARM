#include "dev_encoder.h"
#include "devices.h"
#include "dev_keyboard.h"
#include <stdio.h>

static int encoder_button_pressed;

void dev_encoder_init(void)
{
  encoder_button_pressed = 0;
}

static int send_encoder_event(unsigned int encoder_status)
{
  int button_pressed = encoder_status & 0x80000000 ? 1 : 0;
  int status = encoder_status & 0x7FFFFFFF;
  if (encoder_status & 0x40000000)
    status |= 0x80000000;
  ui_encoder_handler_type handler = device_list[current_keyboard_device]->ui_encoder_handler;
  if (handler)
    return handler(device_config[current_keyboard_device], status, button_pressed);
  return 0;
}

unsigned int encoder_get_status(void)
{
  int counter = encoder_counter;
  int button_pressed = 0;
  if (is_encoder_button_pressed())
  {
    if (!encoder_button_pressed)
    {
      encoder_button_pressed = 1;
      button_pressed = 1;
    }
  }
  else
    encoder_button_pressed = 0;
  encoder_counter = 0;
  return (counter & 0x7FFFFFFF) | (button_pressed ? 0x80000000 : 0);
}

int process_encoder_event(unsigned int encoder_status)
{
  if (current_keyboard_device >= 0 && encoder_status)
    return send_encoder_event(encoder_status);
  return 0;
}
