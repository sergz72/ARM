#include "board.h"
#include <dbgmcu.h>
#include <pll.h>
#include <systick.h>
#include <gpio.h>
#include <nvic.h>

static void GPIOInit(void)
{
  LED_OFF;
  //LED
  GPIO_Init(GPIOC,
            GPIO_Pin_13,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  //adf4351 data
  GPIO_Init(GPIOA,
            GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7,
            GPIO_Mode_OUT,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  adf4351_RESET_SET(0);
  adf4351_FQUD_SET(0);
  adf4351_CLK_CLR(0);
  // adf4351 CLK, FQUD
  GPIO_Init(GPIOB,
            GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_7,
            GPIO_Mode_OUT,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

static void USBInit(void)
{
  //PA11=USBDM,PA12=USBDP
  GPIO_Init(GPIOA,
            GPIO_Pin_11 | GPIO_Pin_12,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG_FS) ;
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG_FS) ;

  RCC->AHB2ENR |= RCC_AHB2LPENR_OTGFSLPEN;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_Init(OTG_FS_IRQn, 1, 3, ENABLE);
}

/**
  * @brief  Setup the microcontroller system
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  /* Set floating point coprosessor access mode. */
//  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

#ifdef DEBUG
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif

  // Init system clock (48 MHz)
  InitRCC(25, 48, FLASH_ACR_LATENCY_1WS, RCC_CFGR_HPRE_DIV1, RCC_CFGR_PPRE2_DIV2, RCC_CFGR_PPRE1_DIV2);

  /* 48MHz / 8 => 6M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  //enable the GPIO clock for port GPIOA
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  GPIOInit();
  USBInit();
}
