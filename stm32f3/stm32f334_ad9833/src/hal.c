#include "board.h"
#include <systick.h>
#include <mcp41_42.h>
#include <spi_soft.h>
#include <ad9833.h>

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
#ifndef DEBUG
    __WFI();
#else
	  ;
#endif
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

void SPI_CS_SET(int channel)
{
  if (channel == SPI_CHANNEL_AD)
  {
    //SET PA12
    GPIOA->BSRR = 0x1000;
  }
  else
  {
    //SET PB6
    GPIOB->BSRR = 0x40;
  }
}

void SPI_CS_CLR(int channel)
{
  if (channel == SPI_CHANNEL_AD)
  {
    //CLR PA12
    GPIOA->BSRR = 0x10000000;
  }
  else
  {
    //CLR PB6
    GPIOB->BSRR = 0x400000;
  }
}

void SPI_CLK_SET(int channel)
{
  if (channel == SPI_CHANNEL_AD)
  {
    //CLR PA15
    GPIOA->BSRR = 0x80000000;
  }
  else
  {
    //SET PA15
    GPIOA->BSRR = 0x8000;
  }
}

void SPI_CLK_CLR(int channel)
{
  if (channel == SPI_CHANNEL_AD)
  {
    //SET PA15
    GPIOA->BSRR = 0x8000;
  }
  else
  {
    //CLR PA15
    GPIOA->BSRR = 0x80000000;
  }
}

void mcp41_42_set(int channel, unsigned char command, unsigned char data)
{
  spi_command(SPI_CHANNEL_MCP, command, &data, NULL, 1, 1);
}

void ad9833_write(int channel, unsigned short data)
{
  unsigned char cmd, d;
  cmd = data >> 8;
  d = data & 0xFF;
  spi_command(SPI_CHANNEL_AD, cmd, &d, NULL, 1, 1);
}
