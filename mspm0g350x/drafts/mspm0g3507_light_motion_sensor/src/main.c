#include "board.h"
#include <dl_gpio.h>
#include <dl_uart_main.h>
#include <dl_timerg.h>
#include <dl_adc12.h>
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <common_printf.h>
#include <string.h>
#include "adc_commands.h"
#include "dac_commands.h"
#include "pwm_commands.h"
#include <pir_sensor.h>

static char usart_buffer[UART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;
static char command_line[200];
static volatile int timer_event;
static volatile int motion_timer;

volatile unsigned int filter_crs;
volatile unsigned short filter_threshold;

void UART_IRQHandler(void)
{
  if (DL_UART_Main_getPendingInterrupt(UART_INSTANCE) == DL_UART_MAIN_IIDX_RX)
  {
    uint8_t gData = DL_UART_Main_receiveData(UART_INSTANCE);
    *usart_buffer_write_p++ = gData;
    if (usart_buffer_write_p == usart_buffer + UART_BUFFER_SIZE)
      usart_buffer_write_p = usart_buffer;
  }
}

void PERIODIC_TIMER_IRQHandler(void)
{
  timer_event = 1;
}

void ADC_INST_IRQHandler(void)
{
  switch (DL_ADC12_getPendingInterrupt(ADC_INST)) {
  case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
    pir_sensor_adc_handler(DL_ADC12_getMemResult(ADC_INST, DL_ADC12_MEM_IDX_0));
    break;
  default:
    break;
  }
}

static int getch_(void)
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

void pir_motion_detected(void)
{
  set_motion_led();
  motion_timer = MOTION_DETECTOR_ON_TIME;
}

static void main_loop(void)
{
  int rc;
  int cnt_led = 0;

  while (1)
  {
    __WFI();
    if (!timer_event)
      continue;
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
      toggle_timer_led();
      cnt_led = 0;
      if (motion_timer)
      {
        motion_timer--;
        if (!motion_timer)
          clear_motion_led();
      }
    }
  }
}

int main(void)
{
  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;

  SystemInit();

  shell_init(common_printf, nullptr);
  register_adc_commands();
  register_dac_commands();
  register_pwm_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  timer_event = 0;
  DL_TimerG_startCounter(PERIODIC_TIMER_INSTANCE);

  filter_crs = 5;
  motion_timer = 0;
  filter_threshold = 8;
  dac_set(DAC_DEFAULT_VALUE);
  DL_ADC12_startConversion(ADC_INST);

  main_loop();
}
