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
static char command_line[200];
volatile int timer_event;
volatile unsigned int counter_low_counter, counter_high_counter, freq_low_counter, freq_high_counter;
unsigned int counter_low, counter_high, counter_freq_low, counter_freq_high;

unsigned int mv_to_12(unsigned int mv)
{
  if (mv >= DAC_REFERENCE_VOLTAGE)
    return 4095;
  return (mv * 4095) / DAC_REFERENCE_VOLTAGE;
}

unsigned int mv_from_12(unsigned int code)
{
  if (code >= 4095)
    return DAC_REFERENCE_VOLTAGE;
  return code * DAC_REFERENCE_VOLTAGE / 4095;
}

static void led_toggle(void)
{
  led_state = !led_state;
  if (led_state)
    LED_ON;
  else
    LED_OFF;
}

static void RAMFUNC main_loop(void)
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

  shell_init(common_printf, NULL);

  register_dac_commands();
  register_pwm_commands();
  register_counters_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  delayms(50);
  UI_Init();

  timer_event = 0;

  CustomMainInit();

  PeriodicTimerStart();

  counter_low = counter_high = counter_freq_low = counter_freq_high = 0;
  start_counters();

  main_loop();
}
