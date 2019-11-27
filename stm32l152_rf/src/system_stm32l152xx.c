#include "board.h"
#include <systick.h>
#include <gpio.h>
#include <spi.h>
#include <dbgmcu.h>
#include <i2c.h>
#include <iwdg.h>
#include <timer.h>
#include <nvic.h>
#include <adc.h>

static void ClockInit(void)
{
  RCC->CR = 0x101; // MSI and HSI are ON, HSI is for ADC
  //waiting for HSI to stabilize
  while (!(RCC->CR & RCC_CR_HSIRDY))
    ;
  // 2.100 MHZ clock (for I2C should be >= 2 MHZ)
  RCC->ICSCR = (RCC->ICSCR & ~RCC_ICSCR_MSIRANGE) | RCC_ICSCR_MSIRANGE_5;
  //switch to MSI
  RCC->CFGR = 0; // MSI is a clock source
  while (RCC->CFGR & RCC_CFGR_SWS)
    ;

  /* Power enable */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  /* Select the Voltage Range 3 (1.2 V) */
  PWR->CR = PWR_CR_VOS_0 | PWR_CR_VOS_1;

  /* Wait Until the Voltage Regulator is ready */
  while((PWR->CSR & PWR_CSR_VOSF) != RESET);
}

static void GPIOInit(void)
{
  //LCD
  GPIO_Init(GPIOB,
            GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9,
            GPIO_Mode_OUT,
            GPIO_Speed_40MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_Init(GPIOA,
            GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15,
            GPIO_Mode_OUT,
            GPIO_Speed_40MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  // GD0, GD2 pins
  GPIO_Init(GPIOA,
            GPIO_Pin_1 | GPIO_Pin_3,
            GPIO_Mode_IN,
            GPIO_Speed_40MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

static void SPI1Init(void)
{
  SPI_InitTypeDef SPI_InitStructure;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  GPIO_Init(GPIOA,
            GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7,
            GPIO_Mode_AF,
            GPIO_Speed_40MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  cc1101_CSN_SET(0);
  // NSS Pin
  GPIO_Init(GPIOA,
            GPIO_Pin_4,
            GPIO_Mode_OUT,
            GPIO_Speed_40MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  // SPI configuration
  SPI_InitStructure.Direction = SPI_DIRECTION_2LINES;
  SPI_InitStructure.Mode = SPI_MODE_MASTER;
  SPI_InitStructure.DataSize = SPI_DATASIZE_8BIT;
  SPI_InitStructure.CLKPolarity = SPI_POLARITY_LOW;
  SPI_InitStructure.CLKPhase = SPI_PHASE_1EDGE;
  SPI_InitStructure.NSS = SPI_NSS_SOFT;
  SPI_InitStructure.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  SPI_InitStructure.FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  SPI_InitStructure.TIMode = FALSE;

  SPI_Init(SPI1, &SPI_InitStructure);
}

static void I2C2Init(void)
{
  I2C_InitTypeDef init;

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);
  GPIO_Init(GPIOB,
            GPIO_Pin_10 | GPIO_Pin_11,
            GPIO_Mode_AF,
            GPIO_Speed_40MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );

  init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  init.ClockSpeed = 100000;
  init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  init.OwnAddress1 = 0;
  init.OwnAddress2 = 0;
  init.DutyCycle = I2C_DUTYCYCLE_2;
  init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

  I2C_Init(I2C2, &init);
}

void IWDGInit(void)
{
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_32);

  IWDG_SetReload(0xFFF); //3542.486 ms

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}

void TIMER7Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = PCLK1_FREQ / 1000;
  TIM_TimeBaseStructure.TIM_Prescaler = 1000 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

  TIM_ARRPreloadConfig(TIM7, ENABLE);

  /* TIM IT enable */
  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

  NVIC_Init(TIM7_IRQn, 3, 1, ENABLE);

  TIM_Cmd(TIM7, ENABLE);
}

void ADC1Init(void)
{
  ADC_InitTypeDef init;

  // VBAT pin
  GPIO_Init(GPIOB,
            GPIO_Pin_14,
            GPIO_Mode_AN,
            GPIO_Speed_40MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV4;
  init.Resolution            = ADC_RESOLUTION_12B;
  init.DataAlign             = ADC_DATAALIGN_RIGHT;
  init.ScanConvMode          = DISABLE;
  init.EOCSelection          = ADC_EOC_SINGLE_CONV;
  init.LowPowerAutoWait      = ADC_AUTOWAIT_UNTIL_DATA_READ;  /* Enable the dynamic low power Auto Delay: new conversion start only when the previous conversion (for regular group) or previous sequence (for injected group) has been treated by user software. */
  init.LowPowerAutoPowerOff  = ADC_AUTOPOWEROFF_IDLE_PHASE;   /* Enable the auto-off mode: the ADC automatically powers-off after a conversion and automatically wakes-up when a new conversion is triggered (with startup time between trigger and start of sampling). */
  init.ChannelsBank          = ADC_CHANNELS_BANK_A;
  init.ContinuousConvMode    = DISABLE;
  init.NbrOfConversion       = 1;
  init.DiscontinuousConvMode = DISABLE;
  init.NbrOfDiscConversion   = 1;
  init.ExternalTrigConv      = ADC_SOFTWARE_START;
  init.DMAContinuousRequests = DISABLE;

  if (!ADC_Init(ADC1, &init))
  {
    while (1);
  }

  ADC_Enable(ADC1);
}

/**
  * @brief  Setup the microcontroller system
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  ClockInit();

  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB);

  //enable the GPIO clock for port GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;

  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

#ifdef DEBUG
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY | DBGMCU_IWDG_STOP | DBGMCU_RTC_STOP, ENABLE);
#endif

  GPIOInit();
  SPI1Init();
  I2C2Init();
  TIMER7Init();
  ADC1Init();
  IWDGInit();
}
