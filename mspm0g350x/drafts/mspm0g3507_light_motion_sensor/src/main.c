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
#include "veml_commands.h"
#include <pir_sensor.h>
#include <veml7700.h>

#ifdef UART_ENABLE
static char usart_buffer[UART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;
static char command_line[200];
#endif
static volatile int timer_event;
static volatile int motion_timer;
static volatile bool motion_detected;

volatile unsigned int filter_crs;
volatile unsigned short filter_threshold;

static bool motion_sensor_active;
static unsigned int veml7700_disable_time;
static unsigned int motion_sensor_disable_time;

#ifdef UART_ENABLE
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
#endif

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

#ifdef UART_ENABLE
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
#endif

void pir_motion_detected(void)
{
  motion_detected = true;
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

#ifdef UART_ENABLE
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
#endif

    cnt_led++;
    if (cnt_led == TIMER_EVENT_FREQUENCY)
    {
#ifdef UART_ENABLE
      toggle_timer_led();
#endif
      cnt_led = 0;
      if (motion_timer)
      {
        motion_timer--;
        if (!motion_timer)
        {
#ifdef UART_ENABLE
          clear_motion_led();
#endif
          pwm_off();
          veml7700_disable_time = 5;
        }
      }
      else if (veml7700_disable_time)
        veml7700_disable_time--;
      else
      {
        unsigned short result;
        bool active;
        rc = veml7700_read(VEML7700_REG_ALS, &result);
        if (rc || result < VEML7700_LOW_THRESHOLD)
        {
          if (!motion_sensor_active)
          {
            motion_sensor_active = true;
            motion_sensor_powerup();
            dac_set(DAC_DEFAULT_VALUE);
            DL_ADC12_startConversion(ADC_INST);
            motion_sensor_disable_time = 20;
          }
        }
        else if (result > VEML7700_HIGH_THRESHOLD)
        {
          if (motion_sensor_active)
          {
            motion_sensor_active = false;
            motion_sensor_shutdown();
            motion_detected = false;
          }
        }
      }
    }

    if (motion_sensor_disable_time)
    {
      motion_sensor_disable_time--;
      motion_detected = false;
    }
    else if (motion_sensor_active & motion_detected)
    {
      motion_detected = false;
      if (motion_timer)
        motion_timer = MOTION_DETECTOR_ON_TIME;
      else
      {
#ifdef UART_ENABLE
        set_motion_led();
#endif
        motion_timer = MOTION_DETECTOR_ON_TIME;
        unsigned int vbat = get_vbat();
        if (vbat < MIN_VBAT)
          vbat = MIN_VBAT;
        pwm_on(VBAT_TO_DUTY(vbat));
      }
    }
  }
}

int main(void)
{
#ifdef UART_ENABLE
  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;
#endif

  SystemInit();

  int rc = veml7700_ex_init(VEML7700_GAIN_2|VEML7700_IT_100ms, VEML7700_PSM_MODE4|VEML7700_PSM_ENABLE);
  if (rc)
  {
    set_motion_led();
    while (1)
      __WFI();
  }

#ifdef UART_ENABLE
  shell_init(common_printf, nullptr);
  register_adc_commands();
  register_dac_commands();
  register_pwm_commands();
  register_veml_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);
#endif

  motion_sensor_active = false;
  veml7700_disable_time = 0;
  motion_sensor_disable_time = 0;

  timer_event = 0;
  DL_TimerG_startCounter(PERIODIC_TIMER_INSTANCE);

  filter_crs = PIR_SENSOR_DEFAULT_FILTER_CRS;
  motion_timer = 0;
  motion_detected = false;
  filter_threshold = PIR_SENSOR_DEFAULT_FILTER_THRESHOLD;

  main_loop();
}
