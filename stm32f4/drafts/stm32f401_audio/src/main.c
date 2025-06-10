#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include "tlv320dac3100_commands.h"
#include "sound.h"
#include <dma.h>

#include "tlv.h"

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
  int rc;

  delayms(5);

  sound_init();
  if (tlv_init())
  {
    LED_ON;
    while(1)
      delayms(100);
  }

  USBStart();

  queue_init(&usb_q, SERIAL_QUEUE_SIZE, 1, (char*)usb_queue_buffer);

  shell_init(usb_printf, NULL);

  register_tlv320dac3100_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  DMA_Cmd(DMA1_Stream5, ENABLE);

  while(1)
  {
    delayms(100);
    if (!getstring_next())
    {
      switch (command_line[0])
      {
        case SHELL_UP_KEY:
          puts_("\r\33[2K$ ");
          getstring_buffer_init(shell_get_prev_from_history());
          break;
        case SHELL_DOWN_KEY:
          puts_("\r\33[2K$ ");
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
