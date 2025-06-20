#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <common_printf.h>
#include "dac_commands.h"
#include "pwm_commands.h"
#include "counters_commands.h"
#include "ui.h"
#include <delay_systick.h>

static int led_state;
static char usart_buffer[USART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;
static char command_line[200];
static volatile int timer_event;
static volatile unsigned int timer1_counter, timer8_counter, timer4_counter, timer3_counter;
unsigned int counter_low, counter_high, counter_freq_low, counter_freq_high;

void __attribute__((section(".RamFunc"))) USART2_IRQHandler(void)
{
  while (USART2->ISR & USART_ISR_RXNE_RXFNE)
  {
    *usart_buffer_write_p++ = (char)USART2->RDR;
    if (usart_buffer_write_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_write_p = usart_buffer;
  }
  USART2->ICR = 0xFFFFFFFF;
}

void __attribute__((section(".RamFunc"))) TIM2_IRQHandler(void)
{
  stop_counters();
  timer_event = 1;
  TIM2->SR = 0;
}

void __attribute__((section(".RamFunc"))) TIM4_IRQHandler(void)
{
  timer4_counter++;
  TIM4->SR = 0;
}

void __attribute__((section(".RamFunc"))) TIM3_IRQHandler(void)
{
  timer3_counter++;
  TIM3->SR = 0;
}

void __attribute__((section(".RamFunc"))) TIM1_UP_TIM16_IRQHandler(void)
{
  timer1_counter++;
  TIM1->SR = 0;
}

void __attribute__((section(".RamFunc"))) TIM8_UP_IRQHandler(void)
{
  timer8_counter++;
  TIM8->SR = 0;
}

static void led_toggle(void)
{
  led_state = !led_state;
  if (led_state)
    LED_ON;
  else
    LED_OFF;
}

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

static void __attribute__((section(".RamFunc"))) update_counters(void)
{
  counter_low = timer1_counter;
  counter_high = timer8_counter;
  counter_freq_low = TIM4->CNT | (timer4_counter << 16);
  counter_freq_high = TIM3->CNT | (timer3_counter << 16);
  timer1_counter = timer8_counter = timer4_counter = timer3_counter = 0;
}

static void __attribute__((section(".RamFunc"))) main_loop(void)
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
      led_toggle();
      cnt_led = 0;
    }
    Process_Timer_Event();
  }
}

int main(void)
{

  led_state = 0;
  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;

  shell_init(common_printf, NULL);

  register_dac_commands();
  register_pwm_commands();
  register_counters_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  delayms(50);
  UI_Init();

  timer_event = 0;
  TIM2->CR1 = TIM_CR1_CEN;

  counter_low = counter_high = counter_freq_low = counter_freq_high = 0;
  start_counters();

  main_loop();
}
