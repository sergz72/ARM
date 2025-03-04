#include "main_loop.h"
#include "dev_keyboard.h"
#include "dev_encoder.h"
#include "devices.h"
#include "ui.h"
#include "settings.h"

static int cnt;

void main_loop_init(void)
{
  cnt = 0;
  read_settings();
}

void main_loop(void)
{
  int update, data_ready;
  unsigned int keyboard_status, encoder_status;

  if (!cnt)
    update = process_current_keyboard_device_switch();
  else
    update = 0;
  BuildDeviceData(cnt++);
  //printf("BuildDeviceData\n");
  keyboard_status = keyboard_get_filtered_status();
  encoder_status = encoder_get_status();
  //printf("keyboard_status: %d\n");
  data_ready = cnt == 10 ? 2 : cnt == 5 ? 1 : 0;
  if (data_ready == 2)
    cnt = 0;
  update |= Process_Timer_Event(data_ready, keyboard_status, encoder_status);
  //printf("Process_Timer_Event\n");
  if (update && !displayIsOff)
    LED_Update();
}
