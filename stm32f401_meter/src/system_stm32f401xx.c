#include "board.h"
#include <dbgmcu.h>
#include <pll.h>
#include <systick.h>
#include <gpio.h>
#include <nvic.h>
#include <i2c_soft.h>

static void GPIOInit(void)
{
  //LED
  GPIO_Init(GPIOC,
            GPIO_Pin_13,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  // LCD
  GPIO_Init(GPIOC,
            GPIO_Pin_14 | GPIO_Pin_15,
            GPIO_Mode_OUT,
            GPIO_Speed_25MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_Init(GPIOA,
            GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7,
            GPIO_Mode_OUT,
            GPIO_Speed_25MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_Init(GPIOB,
            GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2,
            GPIO_Mode_OUT,
            GPIO_Speed_25MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  LED_OFF;
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

static void I2C3Init(void)
{
  GPIO_Init(GPIOB,
            GPIO_Pin_6 | GPIO_Pin_7,
            GPIO_Mode_OUT,
            GPIO_Speed_100MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );

  i2c_soft_init(2);
}

static void I2C4Init(void)
{
  i2c_soft_init(3);

  //SCL
  GPIO_Init(GPIOB,
            GPIO_Pin_9,
            GPIO_Mode_OUT,
            GPIO_Speed_100MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );

  keyboard_present = (GPIOB->IDR & 0x200) == 0;
  if (!keyboard_present)
  {
    //SDA
    GPIO_Init(GPIOB,
              GPIO_Pin_8,
              GPIO_Mode_OUT,
              GPIO_Speed_100MHz,
              GPIO_OType_OD,
              GPIO_PuPd_UP
    );
  }
  else
  {
    SPI3_CLK_SET;
    SPI3_CS_SET;
    //DIO
    GPIO_Init(GPIOB,
              GPIO_Pin_8,
              GPIO_Mode_OUT,
              GPIO_Speed_100MHz,
              GPIO_OType_OD,
              GPIO_PuPd_NOPULL
    );
    //STB
    GPIO_Init(GPIOB,
              GPIO_Pin_5,
              GPIO_Mode_OUT,
              GPIO_Speed_100MHz,
              GPIO_OType_PP,
              GPIO_PuPd_NOPULL
    );
    //CLK
    GPIO_Init(GPIOA,
              GPIO_Pin_15,
              GPIO_Mode_OUT,
              GPIO_Speed_100MHz,
              GPIO_OType_PP,
              GPIO_PuPd_NOPULL
    );
  }
}

static void I2C1Init(void)
{
  GPIO_Init(GPIOB,
            GPIO_Pin_3 | GPIO_Pin_10,
            GPIO_Mode_OUT,
            GPIO_Speed_100MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );

  i2c_soft_init(0);
}

static void I2C2Init(void)
{
  GPIO_Init(GPIOB,
            GPIO_Pin_4,
            GPIO_Mode_OUT,
            GPIO_Speed_100MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );

  GPIO_Init(GPIOA,
            GPIO_Pin_8,
            GPIO_Mode_OUT,
            GPIO_Speed_100MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );

  i2c_soft_init(1);
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
  I2C1Init();
  I2C2Init();
  I2C3Init();
  I2C4Init();
}
