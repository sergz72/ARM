#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <usb_func.h>

int led_state;

static char command_line[200];
Queue usb_q;
static unsigned char usb_queue_buffer[SERIAL_QUEUE_SIZE];

static int getch_(void)
{
  __disable_irq();
  unsigned char *v = queue_peek(&usb_q);
  if (v)
  {
    queue_pop(&usb_q);
    __enable_irq();
    return *v;
  }
  __enable_irq();
  return EOF;
}

static void led_toggle(void)
{
  led_state = !led_state;
  if (led_state)
    LED_ON;
  else
    LED_OFF;
}

int main(void)
{
  int rc;
  led_state = 0;

  USBStart();

  queue_init(&usb_q, SERIAL_QUEUE_SIZE, 1, (char*)usb_queue_buffer);

  shell_init(usb_printf, NULL);

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    delayms(100);
    led_toggle();
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
  }
}
