#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <cc1101_commands.h>
#include "scd41_commands.h"
#include "date_commands.h"
#include <usb.h>
#include <adc.h>

static bool led_state;
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
  int led_counter = 0;
  bool vbus_present = false;

  led_state = false;

  queue_init(&usb_q, SERIAL_QUEUE_SIZE, 1, (char*)usb_queue_buffer);

  shell_init(usb_printf, nullptr);
  register_cc1101_commands();
  register_scd41_commands();
  register_date_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    delayms(100);
    ADC_Init(ADC4, ADC4_COMMON, 1, 1);
    ADC_Enable(ADC4);
    if (adc_getvbus() >= VBUS_MIN)
    {
      if (!vbus_present)
      {
        vbus_present = true;
        USB_Init();
        USBStart();
      }
    }
    else if (vbus_present)
    {
      vbus_present = false;
      USB_DisableGlobalInt();
      USB_Deinit();
    }
    ADC_Deinit(ADC4, ADC4_COMMON);
    if (led_counter == 9)
    {
      led_counter = 0;
      led_toggle();
    }
    else
      led_counter++;
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
