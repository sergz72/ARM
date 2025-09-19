#include "board.h"
#include <dl_gpio.h>
#include <systick.h>
#include <dl_uart_main.h>
#include <dl_timerg.h>
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <common_printf.h>
#include "adc_commands.h"
#include "dac_commands.h"
#include "multimeter_commands.h"

static char usart_buffer[UART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;
static char command_line[200];

void __attribute__((section(".ramfunc"))) UART_IRQHandler(void)
{
  if (DL_UART_Main_getPendingInterrupt(UART_INSTANCE) == DL_UART_MAIN_IIDX_RX)
  {
    uint8_t gData = DL_UART_Main_receiveData(UART_INSTANCE);
    *usart_buffer_write_p++ = gData;
    if (usart_buffer_write_p == usart_buffer + UART_BUFFER_SIZE)
      usart_buffer_write_p = usart_buffer;
  }
}

static void toggle_led(void)
{
  DL_GPIO_togglePins(LED_GPIO, LED_PIN);
}

static int __attribute__((section(".ramfunc"))) getch_(void)
{
  if (usart_buffer_write_p != usart_buffer_read_p)
  {
    char c = *usart_buffer_read_p++;
    if (usart_buffer_read_p == usart_buffer + UART_BUFFER_SIZE)
      usart_buffer_read_p = usart_buffer;
    return c;
  }
  return EOF;
}

void __attribute__((section(".ramfunc"))) puts_(const char *s)
{
  for (;;)
  {
    char c = *s++;
    if (!c)
      return;
    DL_UART_Main_transmitDataBlocking(UART_INSTANCE, c);
  }
}

int main(void)
{
  int rc;
  int cnt_led = 0;

  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;

  SystemInit();

  shell_init(common_printf, NULL);
  register_dac_commands();
  register_adc_commands();
  register_multimeter_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    delayms(10);

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
          common_printf("shell_execute returned %d\n$ ", rc);
        break;
      }
    }

    if (cnt_led == 49)
    {
      toggle_led();
      cnt_led = 0;
    }
    else
      cnt_led++;
  }
}
