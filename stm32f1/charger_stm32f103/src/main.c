#include "board.h"
#include <pll.h>
#include <systick.h>
#include <adc.h>
#include <timer.h>
#include <dma.h>
#include <nvic.h>
#include <string.h>

#define ADC1_DR_Address    ((uint32_t)0x4001244C)

char input_buffer[CMD_BUF_SIZE];
char *input_buffer_p;
char command_buffer[CMD_BUF_SIZE];
char *command_buffer_p;
unsigned int echo_enabled, command_ready;
//unsigned int TIM_PERIOD;
Status status;
__IO uint16_t ADC1ConvertedValue[MAX_CHANNEL*2];

int Sys_Tick;

void TIM1_UP_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		//USART_To_USB_Send_Data('0');
    // Start ADC1 Software Conversion 
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  }
}

void DMA1_Channel1_IRQHandler(void)
{
	int U1, U2, U, I, i;
	Channel *c;
	
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
  	DMA_ClearITPendingBit(DMA1_IT_GL1);
		U1 = (ADC1ConvertedValue[0] * VREF / 4096) * (RL + RH00) / RL;
		I = U1 * 1000 / RI;
		if (I < 0)
			I = 0;
		U = (ADC1ConvertedValue[1] * VREF / 4096) * (RL + RH01) / RL;
		U2 = U - U1;
		if (U2 < 0)
			U2 = 0;
//  	status.channel[0].set.U = ADC1ConvertedValue[1];
//	  status.channel[0].set.I = ADC1ConvertedValue[0];
  	status.channel[0].fact.U = U2;
	  status.channel[0].fact.I = I;
		U1 = (ADC1ConvertedValue[2] * VREF / 4096) * (RL + RH10) / RL;
		I = (U - U1) * 1000 / RI;
		if (I < 0)
			I = 0;
		U1 = U;
    U = (ADC1ConvertedValue[3] * VREF / 4096) * (RL + RH11) / RL;
		U2 = U - U1;
		if (U2 < 0)
			U2 = 0;
//  	status.channel[1].set.U = ADC1ConvertedValue[3];
//	  status.channel[1].set.I = ADC1ConvertedValue[2];
	  status.channel[1].fact.U = U2;
	  status.channel[1].fact.I = I;
		U1 = (ADC1ConvertedValue[4] * VREF / 4096) * (RL + RH20) / RL;
		I = (U - U1) * 1000 / RI;
		if (I < 0)
			I = 0;
		U1 = U;
		U = (ADC1ConvertedValue[5] * VREF / 4096) * (RL + RH21) / RL;
		U2 = U - U1;
		if (U2 < 0)
			U2 = 0;
//  	status.channel[2].set.U = ADC1ConvertedValue[5];
//	  status.channel[2].set.I = ADC1ConvertedValue[4];
	  status.channel[2].fact.U = U2;
	  status.channel[2].fact.I = I;
		
		for (i = 0; i < MAX_CHANNEL; i++)
		{
			c = &status.channel[i];
			if ((c->set.I != 0) && (c->set.U != 0))
			{
				// U stabilisation
/*				U = c->set.U - c->fact.U + 25;
				if (U > 50)
					U = 50;
				else
				{
					if (U < 0)
					  U = 0;
				}
				// I stabilisation
				I = c->set.I - c->fact.I + 25;
				if (I > 50)
					I = 50;
				else
				{
					if (I < 0)
					  I = 0;
				}
				if (I < U)
					U = I;*/
				U = c->duty;
				if ((c->set.U < c->fact.U) || (c->set.I < c->fact.I))
					U--;
				else
				{
  				if ((c->set.U > c->fact.U) && (c->set.I > c->fact.I))
	  				U++;
				}
				
				setDuty(i, U);
			}
		}
	}
}

void setDuty(int channel, int duty)
{
	if (duty > _PWM_PERIOD_ / 2)
		duty = _PWM_PERIOD_ / 2;
	if (duty < 0)
		duty = 0;
	status.channel[channel].duty = duty;
	
	switch (channel)
	{
		case 0: TIM_SetCompare2(TIM3, duty); break;
		case 1: TIM_SetCompare3(TIM3, duty); break;
		case 2: TIM_SetCompare4(TIM3, duty); break;
	}
}

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
	/* 9000000/reload overflows per second */
	systick_set_reload(us * 9);

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
    __wfi();
}

void delay(unsigned int us)
{
  systick_start(us);
  systick_wait();
  systick_stop();
}

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

void pwm_init(void)
{
  TIM_SetCompare1(TIM3, 0);
  TIM_SetCompare2(TIM3, 0);
  TIM_SetCompare3(TIM3, 0);
  TIM_SetCompare4(TIM3, 0);
}

void SystemInit(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

	memset(&status, 0, sizeof(status));
	
  // system clock = pllin x 9 (72 MHz)
  InitRCC(9);

  //enable the GPIO clock for port GPIOA
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

  //enable the GPIO clock for port GPIOC
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
  //alternate function io enable
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  //enable the clock for Timer 3
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  //enable the clock for Timer 1
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  //enable the clock for ADC1
//  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  //enable the clock for DMA
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;

  GPIOA->CRL = 0; // PA0-PA7: Analog mode
  GPIOA->CRH = 0x44444424; // PA9: Output mode, max speed 2 MHz; General purpose output push-pull

  GPIOC->CRL = 0xBB444444; // PC6,PC7: Output mode, max speed 50 MHz; Alternate function output push-pull
  GPIOC->CRH = 0x444444BB; // PC8,PC9: Output mode, max speed 50 MHz; Alternate function output push-pull

	// TIM3 remap to PC6-PC9
  AFIO->MAPR &= ~AFIO_MAPR_TIM3_REMAP; 
  AFIO->MAPR |= AFIO_MAPR_TIM3_REMAP_1 | AFIO_MAPR_TIM3_REMAP_0; 
	
  /* 72MHz / 8 => 9000000 counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8); 

  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC1ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = MAX_CHANNEL*2;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  /* PCLK2 is the APB2 clock */
  /* ADCCLK = PCLK2/6 = 72/6 = 12MHz*/
  ADC_CLKConfig(ADC_CLK_Div6);
  ADC_ClockEnable(ADC1, ENABLE);
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//ADC_ExternalTrigConv_T1_CC1;//;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = MAX_CHANNEL*2;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_Calibration2(ADC1);

  /* ADC1 regular channel configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_13Cycles5);
/*  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_13Cycles5);*/

  /* Enable ADC1 external trigger */ 
  ADC_ExternalTrigConvCmd(ADC1, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

 	DMA_ClearITPendingBit(DMA1_IT_GL1);
  /* Enable DMA Transfer complete interrupt */
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

//	TIM_PERIOD = _TIM_PERIOD_;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = _PWM_PERIOD_; // 20 kHz
  TIM_TimeBaseStructure.TIM_Prescaler = 0; // no prescaling
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM3, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
	pwm_init();
  /* TIM3 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM3, ENABLE);

  TIM_TimeBaseStructure.TIM_Period = _TIM1_PERIOD_; // 20 kHz
  TIM_TimeBaseStructure.TIM_Prescaler = _TIM1_PRESCALER_; // divide by 20
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM1, ENABLE);

  NVIC_Init(TIM1_UP_IRQn, 0, 1, ENABLE);

  NVIC_Init(DMA1_Channel1_IRQn, 0, 2, ENABLE);
}

int main(void)
{
  char c;
	static char temp[RESPONSE_BUFFER_SIZE];
  int command_size;
	
	input_buffer_p = input_buffer;
  command_buffer_p = command_buffer;
	echo_enabled = 0;
  command_ready = 0;

	USB_CDC_Init();

  /* TIM IT enable */
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM1, ENABLE);
	
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
