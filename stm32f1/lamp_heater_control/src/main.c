#include "board.h"
#include <pll.h>
#include <systick.h>
#include <adc.h>

char input_buffer[CMD_BUF_SIZE];
char *input_buffer_p;
char command_buffer[CMD_BUF_SIZE];
char *command_buffer_p;
unsigned int echo_enabled, command_ready;

/*int Sys_Tick;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
	// 9000000/reload overflows per second
	systick_set_reload(us * 9);

	systick_interrupt_enable();
	// start counting
	systick_counter_enable();
}

void systick_stop(void)
{
	// stop counting
	systick_counter_disable();
	systick_interrupt_disable();
}

void systick_wait(void)
{
  Sys_Tick = 0;
  while (!Sys_Tick)
    __wfi();
}

void delay(unsigned int us)
{
  systick_start(us);
  systick_wait();
  systick_stop();
}*/

void rn(void)
{
  USART_To_USB_Send_Data('\r');
  USART_To_USB_Send_Data('\n');
}

void serial_receive(char c)
{
  if (c == 8) // backspace
  {
    if (input_buffer_p > input_buffer)
    {
      input_buffer_p--;
      if (echo_enabled)
        USART_To_USB_Send_Data(c);
    }
    return;
  }

  if (c == '\n')
    return;

  if (c != '\r')
  {
    if (input_buffer_p == &input_buffer[CMD_BUF_SIZE-1])
      return;

    *input_buffer_p++ = c;
    if (echo_enabled)
      USART_To_USB_Send_Data(c);
  }
  else
  {
    *input_buffer_p = 0;
    command_ready = 1;
    if (echo_enabled)
      rn();
  }
}

/*******************************************************************************
* Function Name  : USB_To_USART_Send_Data.
* Description    : gets the received data from USB
* Input          : data_buffer: data address.
                   Nb_bytes: number of bytes received.
* Return         : none.
*******************************************************************************/
void USB_To_USART_Send_Data(uint8_t* data_buffer, uint8_t Nb_bytes)
{
  while (Nb_bytes--)
    serial_receive(*data_buffer++);
}

void SystemInit(void)
{
  // system clock = pllin x 9 (72 MHz)
  InitRCC(9);

  //enable the GPIO clock for port GPIOA
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  //enable the GPIO clock for port GPIOC
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
  //alternate function io enable
//  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

	HEATER0_OFF;
	HEATER1_OFF;
  GPIOA->CRL = 0x44442240; // PA0: Analog mode, PA2, PA3: Output mode, max speed 2 MHz
  GPIOC->CRH = 0x44244444; // PC13: Output mode, max speed 2 MHz

  /* 72MHz / 8 => 9000000 counts per second */
//  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8); 

  /* PCLK2 is the APB2 clock */
  /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
  ADC_CLKConfig(ADC_CLK_Div6);
  ADC_ClockEnable(ADC1, ENABLE);
  ADC_Calibration(ADC1);
}

int main(void)
{
  char c;
	static char temp[RESPONSE_BUFFER_SIZE];
  int command_size;
	
	input_buffer_p = input_buffer;
  command_buffer_p = command_buffer;
	echo_enabled = 1;
  command_ready = 0;

	USB_CDC_Init();

  while (1)
  {
    if (command_ready)
    {
      input_buffer_p = input_buffer;
			command_size = 0;
      while ((c = *input_buffer_p++) != 0)
			{
        *command_buffer_p++ = c;
				command_size++;
			}
      input_buffer_p = input_buffer;
      command_ready = 0;
      *command_buffer_p = 0;
      command_buffer_p = command_buffer;
      process_command(command_buffer, temp, command_size);
      USB_puts(temp);
    }
  }
}
