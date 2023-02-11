#include "board.h"
#include "ui.h"
#include "devices.h"
#include "dev_keyboard.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>

static char command_line[200];
Queue usb_q;
static unsigned char usb_queue_buffer[SERIAL_QUEUE_SIZE];

static int getch_(void)
{
  __disable_irq();
  unsigned char *v = (unsigned char *)queue_peek(&usb_q);
  if (v)
  {
    queue_pop(&usb_q);
    __enable_irq();
    return *v;
  }
  __enable_irq();
  return EOF;
}

int main(void)
{
  int cnt, data_ready;
  unsigned int keyboard_status;
  int rc;

  USBStart();

  queue_init(&usb_q, SERIAL_QUEUE_SIZE, 1, (char*)usb_queue_buffer);

  shell_init(usb_printf, NULL);

  cnt = keyboard_init();
  //keyboard_set_display("12345678");

  delayms(1000); // time for peripheral devices to initialize

  BuildDeviceList(cnt == 0 ? MAX_DEVICES - 1 : -1);

  build_keyboard_device_list(cnt == 0);

  BuildShellCommands();

  UI_Init();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  cnt = 0;
  while(1)
  {
    delayms(100);
    if (!getstring_next())
    {
      switch (command_line[0])
      {
        case SHELL_UP_KEY:
          puts("\r\33[2K$ ");
          getstring_buffer_init(shell_get_prev_from_history());
          break;
        case SHELL_DOWN_KEY:
          puts("\r\33[2K$ ");
          getstring_buffer_init(shell_get_next_from_history());
          break;
        default:
          rc = shell_execute(command_line);
          if (rc == 0)
            puts_("OK\r\n$ ");
          else if (rc < 0)
            puts_("Invalid command line\r\n$ ");
          else
            usb_printf("shell_execute returned %d\n$ ", rc);
          break;
      }
    }
    BuildDeviceData(cnt++);
    keyboard_status = keyboard_get_filtered_status();
    data_ready = cnt == 10;
    if (data_ready)
      cnt = 0;
    Process_Timer_Event(data_ready, keyboard_status);
  }
}
