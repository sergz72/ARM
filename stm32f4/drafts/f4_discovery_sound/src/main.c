#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>

#include "cs43l22.h"
#include "cs43l22_commands.h"
#include "dac_commands.h"
#include "sound.h"

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

static void tests(void)
{
  //delayms(5);
  //unsigned char id;
  //cs43l22_read_id(&id);

  //dac_set_frequency(1000);

  //shell_execute("cs43l22 passthrough 0");
}

int main(void)
{
  int rc;

  sound_init();

  delayms(5);
  //CS43L22_RESET_SET;
  delayms(5);
  cs43l22_power_control2(CS43L22_POWER_CONTROL_ALWAYS_ON, CS43L22_POWER_CONTROL_ALWAYS_ON,
                          CS43L22_POWER_CONTROL_ALWAYS_OFF, CS43L22_POWER_CONTROL_ALWAYS_OFF);
  cs43l22_set_beep_frequency_and_on_time(1000, 1200);
  cs43l22_set_beep_volume_and_off_time(0, 1200);

  USBStart();

  queue_init(&usb_q, SERIAL_QUEUE_SIZE, 1, (char*)usb_queue_buffer);

  shell_init(usb_printf, NULL);

  register_cs43l22_commands();
  register_dac_commands();

  tests();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

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
