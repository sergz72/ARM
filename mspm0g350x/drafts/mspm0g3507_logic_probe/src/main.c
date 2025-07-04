#include "board.h"
#include <dl_gpio.h>
#include <systick.h>
#include <dl_uart_main.h>
#include <dl_timerg.h>
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <common_printf.h>
#include "dac_commands.h"
#include "pwm_commands.h"
#include "counters_commands.h"
#include "ui.h"

static char usart_buffer[UART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;
static char command_line[200];
static volatile int timer_event;
static volatile unsigned int counter_low_counter, counter_high_counter, freq_low_counter, freq_high_counter;
unsigned int counter_low, counter_high, counter_freq_low, counter_freq_high;

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

void __attribute__((section(".ramfunc"))) COUNTER_LOW_IRQHandler(void)
{
  counter_low_counter++;
}

void __attribute__((section(".ramfunc"))) COUNTER_HIGH_IRQHandler(void)
{
  counter_high_counter++;
}

void __attribute__((section(".ramfunc"))) TIMG12_IRQHandler(void)
{
  stop_counters();
  timer_event = 1;
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

static void __attribute__((section(".RamFunc"))) update_counters(void)
{
  counter_low = counter_low_counter;
  counter_high = counter_high_counter;
  //counter_freq_low = TIM4->CNT | (timer4_counter << 16);
  //counter_freq_high = TIM3->CNT | (timer3_counter << 16);
  counter_low_counter = counter_high_counter = freq_high_counter = freq_low_counter = 0;
}

static void __attribute__((section(".ramfunc"))) main_loop(void)
{
  int rc;
  int cnt_led = 0;

  while (1)
  {
    __WFI();
    if (!timer_event)
      continue;
    update_counters();
    start_counters();
    timer_event = 0;

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

    cnt_led++;
    if (cnt_led == TIMER_EVENT_FREQUENCY)
    {
      toggle_led();
      cnt_led = 0;
    }
    Process_Timer_Event();
  }

}

int main(void)
{
  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;

  SystemInit();

  shell_init(common_printf, NULL);

  register_dac_commands();
  register_pwm_commands();
  register_counters_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  delayms(50);
  //UI_Init();

  timer_event = 0;
  DL_TimerG_startCounter(TIMG12);

  counter_low = counter_high = counter_freq_low = counter_freq_high = 0;
  start_counters();

  main_loop();
}
