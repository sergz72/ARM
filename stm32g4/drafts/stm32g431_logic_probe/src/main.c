#include "board.h"
#include <delay_systick.h>
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <string.h>
#include <usart.h>
#include <common_printf.h>

static int led_state;
static char usart_buffer[USART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;
static char command_line[200];

void USART2_IRQHandler(void)
{
  while (USART2->ISR & USART_ISR_RXNE_RXFNE)
  {
    *usart_buffer_write_p++ = (char)USART2->RDR;
    if (usart_buffer_write_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_write_p = usart_buffer;
  }
  USART2->ICR = 0xFFFFFFFF;
}

static void led_toggle(void)
{
  led_state = !led_state;
  if (led_state)
    LED_ON;
  else
    LED_OFF;
}

/*static void usart_proc(void)
{
  while (usart_buffer_write_p != usart_buffer_read_p && USART2->ISR & USART_ISR_TXE_TXFNF)
  {
    char c = *usart_buffer_read_p++;
    if (usart_buffer_read_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_read_p = usart_buffer;
    USART2->TDR = c;
  }
}*/

static int getch_(void)
{
  if (usart_buffer_write_p != usart_buffer_read_p)
  {
    char c = *usart_buffer_read_p++;
    if (usart_buffer_read_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_read_p = usart_buffer;
    return c;
  }
  return EOF;
}

int main(void)
{
  int rc;

  led_state = 0;
  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;
  int cnt = 0;

  shell_init(common_printf, NULL);

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    delayms(100);
    //usart_proc();

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

    cnt++;
    if (cnt == 10)
    {
      led_toggle();
      cnt = 0;
    }
  }
}
