#include "board.h"
#include <systick.h>
#include <spi.h>
#include <i2c.h>
#include <th02.h>
#include <timer.h>
#include <nvic.h>
#include <exti.h>
#include <cc1101.h>
#include <gpio.h>

volatile int Sys_Tick;
volatile int second_interrupt;
volatile int packet_received;

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

void ST7066_DATA_SET(unsigned int d)
{
  d <<= 2U;
  GPIOB->ODR = (GPIOB->ODR & 0xFC07U) | (d & 0x3F8U);
  if (d & 4U)
    //set PA15
    GPIOA->BSRR = 0x8000;
  else
    //clr PA15
    GPIOA->BSRR = 0x80000000;
}

unsigned int th02Read(unsigned char register_no, unsigned char *pData, int size)
{
  return I2C_Mem_Read(I2C2, TH02_ADDRESS, register_no, I2C_MEMADD_SIZE_8BIT, pData, size, 0xFFFF);
}

unsigned int th02Write(unsigned char register_no, unsigned char value)
{
  unsigned char data[2];
  data[0] = register_no;
  data[1] = value;
  return I2C_Master_Transmit(I2C2, TH02_ADDRESS, data, 2, 0xFFFF);
}

void st7066delay(void)
{
  int i;
  for (i = 0; i < 2; i++)
    ;
}

void cc1101_GD0_Interrupt_Init(void)
{
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
  EXTI_Init(EXTI_Line1, EXTI_Mode_Interrupt, EXTI_Trigger_Rising, ENABLE);
  NVIC_Init(EXTI1_IRQn, 0x0F, 0x0F, ENABLE);
}

unsigned int cc1101_RW(unsigned int device_num, unsigned char *txdata, unsigned char *rxdata, unsigned int size)
{
  unsigned int rc;

  if (size < 2)
    return 0;

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
    return 0; // timeout

  if (size > 2)
    txdata[0] |= CC1101_BURST;

  cc1101_CSN_CLR(device_num);

  rc = SPI_TransmitReceive(SPI1, txdata, rxdata, size, CC1101_TIMEOUT);

  cc1101_CSN_SET(device_num);

  return rc;
}

unsigned int cc1101_strobe(unsigned int device_num, unsigned char data, unsigned char *status)
{
  unsigned int rc;

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
    return 0; // timeout

  cc1101_CSN_CLR(device_num);

  rc = SPI_TransmitReceive(SPI1, &data, status, 1, CC1101_TIMEOUT);

  cc1101_CSN_SET(device_num);

  return rc;
}

void TIM7_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
  {
    second_interrupt = 1;
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
  }
}

void EXTI1_IRQHandler(void)
{
  packet_received = 1;
  EXTI_ClearITPendingBit(EXTI_Line1);
}
