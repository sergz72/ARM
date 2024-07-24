#include "board.h"
#include <gpio.h>
#include <timer.h>
#include <adc.h>
#include <nvic.h>
#include <i2c.h>

int SSD1306_I2C_Write(int num_bytes, unsigned char control_byte, unsigned char *buffer)
{
  int timeout, i;

  timeout = 10000;  
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_BUSY) != RESET)
	{
	  if((timeout--) == 0)
			return 1;
	}

  /* Configure slave address, nbytes, reload and generate start */
  I2C_TransferHandling(I2C1, SSD1306_I2C_ADDRESS, num_bytes + 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
  
	/* Wait until TXIS flag is set */
	timeout = 10000;  
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}

	/* Send data */
	I2C_SendData(I2C1, control_byte);

  for (i = 0; i < num_bytes; i++)
  {
    /* Wait until TXIS flag is set */
    timeout = 10000;  
    while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET)
    {
      if((timeout--) == 0)
				return 1;
    }
  
    /* Send data */
    I2C_SendData(I2C1, *buffer++);
  }

  /* Wait until STOPF flag is set */
  timeout = 10000;
  while(I2C_GetFlagStatus(I2C1, I2C_ISR_STOPF) == RESET)
  {
    if((timeout--) == 0)
			return 1;
  }   
  
  /* Clear STOPF flag */
  I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);
  
  return 0;
}

void SystemInit (void)
{    
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  ADC_InitTypeDef ADC_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;

	//enable the GPIO clock for port GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  //enable the clock for Timer 14
  RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
  //enable the clock for ADC1
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  //enable the clock for I2C1
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  /* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_4);
  
  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);
  
  /* Configure I2C pins */
  GPIO_Init(GPIOA,
            GPIO_Pin_9 | GPIO_Pin_10,
            GPIO_Mode_AF,
            GPIO_Speed_2MHz,
            GPIO_OType_OD,
            GPIO_PuPd_NOPULL
           );
	
  // analog pins
  GPIO_Init(GPIOA,
            GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2,
            GPIO_Mode_AN,
            GPIO_Speed_Level_1,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
           );

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 2000; // 5 Hz
  TIM_TimeBaseStructure.TIM_Prescaler = (unsigned short) (BOARD_PCLK / 10000) - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM14, ENABLE);
  /* TIM IT enable */
  TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

  /* ADCCLK = PCLK/2 = 4MHz*/
  ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);

  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  /* Enable the ADC peripheral */
  ADC_Cmd(ADC1, ENABLE);     
  /* Wait the ADRDY flag */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 

  /* I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter = 0x00;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Timing = I2C_TIMING;
  
  /* Apply I2C configuration after enabling it */
  I2C_Init(I2C1, &I2C_InitStructure);
   
  /* I2C Peripheral Enable */
  I2C_Cmd(I2C1, ENABLE);

  TIM_Cmd(TIM14, ENABLE);
  NVIC_Init(TIM14_IRQn, 3, 1, ENABLE);
}
