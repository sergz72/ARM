#include "board.h"
#include <systick.h>
#include <spi.h>
#include <ad9833.h>
#include <mcp41_42.h>

volatile int Sys_Tick;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
	systick_set_reload(us * SYSTICK_MULTIPLIER);

	systick_interrupt_enable();
	/* start counting */
	systick_counter_enable();
}

void systick_stop(void)
{
	/* stop counting */
	systick_counter_disable();
	systick_interrupt_disable();
}

void systick_wait(void)
{
  Sys_Tick = 0;
  while (!Sys_Tick)
    __WFI();
}

void delay(unsigned int us)
{
  systick_start(us);
  systick_wait();
  systick_stop();
}

void delayms(unsigned int ms)
{
  while (ms--)
    delay(1000);
}

void mcp41_42_set(int channel, unsigned char command, unsigned char data)
{
  SPI_SetCpolCpha(SPI1, 0, 1);
  CS_CLR;
  SPI_Send(SPI1, (command << 8) | data);
  SPI_BusyWait(SPI1);
  CS_SET;
}

void ad9833_write(int channel, unsigned short data)
{
  SPI_SetCpolCpha(SPI1, 1, 0);
  FSY_CLR;
  SPI_Send(SPI1, data);
  SPI_BusyWait(SPI1);
  FSY_SET;
}
