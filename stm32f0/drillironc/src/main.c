#include "board.h"
#include "drill.h"
#include "heater.h"
#include <serial.h>
#include <adc.h>
#include <systick.h>
#include <timer.h>
#include <nvic.h>

int (*serial_handler)(char);

char command;
char send_buffer[20];
char *write_buffer_p, *read_buffer_p;

unsigned int timer_event;

void TIM14_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET)
  {
		timer_event = 1;
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
  }
}

void char_send(char c)
{
  serial_send(SERIAL_CONSOLE, c);
}

void str_send(char *s)
{
	while (*s)
		char_send(*s++);
}

void error(void)
{
	serial_handler = NULL;
  *write_buffer_p++ = 'E';
}

void OK(void)
{
	serial_handler = NULL;
  *write_buffer_p++ = 'K';
}

void END(void)
{
	serial_handler = NULL;
}

void serial_receive(char c)
{
	if (!serial_handler)
	{
		if (c == 'I')
			*write_buffer_p++ = 'I';
		else
		{
			command = c;
			if (!drill_handler(c))
			{
				if (!heater_handler(c))
					error();//error
			}
	  }
	}
  else
		if (!serial_handler(c))
			error();
}

int Sys_Tick;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
	/* 6000000/reload overflows per second */
	systick_set_reload(us * 6);
	systick_set_value(0);
	systick_get_countflag();
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

/*void delay2(unsigned int us)
{
  systick_start(us);
  while (!systick_get_countflag());
  systick_stop();
}*/

void SystemPostInit(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  ADC_InitTypeDef ADC_InitStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

	//enable the GPIO clock for port GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  //enable the GPIO clock for port GPIOB
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  //enable the clock for Timer 14
  RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
  //enable the clock for Timer 2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // drill button pin
  GPIO_Init(DRILL_BUTTON_PORT,
            DRILL_BUTTON_PIN,
            GPIO_Mode_IN,
            GPIO_Speed_Level_1,
            GPIO_OType_PP,
            GPIO_PuPd_UP
           );
  // iron button pin
  GPIO_Init(IRON_BUTTON_PORT,
            IRON_BUTTON_PIN,
            GPIO_Mode_IN,
            GPIO_Speed_Level_1,
            GPIO_OType_PP,
            GPIO_PuPd_UP
           );
  // analog pins
  GPIO_Init(GPIOA,
            GPIO_Pin_7 | GPIO_Pin_5 | GPIO_Pin_6,
            GPIO_Mode_AN,
            GPIO_Speed_Level_1,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
           );
  // PWM pin
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_2);
  GPIO_Init(GPIOA,
            GPIO_Pin_0,
            GPIO_Mode_AF,
            GPIO_Speed_Level_3,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
           );
  // HEATER pins
	heater_off(1);
	heater_off(2);
	heater_off(3);
  GPIO_Init(GPIOA,
            GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3,
            GPIO_Mode_OUT,
            GPIO_Speed_Level_1,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
           );
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  GPIO_Init(GPIOA,
            GPIO_Pin_9 | GPIO_Pin_10,
            GPIO_Mode_AF,
            GPIO_Speed_Level_2,
            GPIO_OType_PP,
            GPIO_PuPd_UP
           );

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 100; // 100 Hz
  TIM_TimeBaseStructure.TIM_Prescaler = (unsigned short) (BOARD_PCLK / 10000) - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);
  TIM_ARRPreloadConfig(TIM14, ENABLE);
  /* TIM IT enable */
  TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = TIM_PERIOD;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM configuration */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_OC2Init(TIM2, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM2, ENABLE);
  /* TIM2 enable */
  TIM_Cmd(TIM2, ENABLE);

  /* 48MHz / 8 => 6000000 counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8); 

  /* ADCCLK = PCLK/4 = 48/4 = 12MHz*/
  ADC_ClockEnable(ADC1, ENABLE);
  ADC_CLKConfig(ADC_CLK_Div4);

  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
  ADC_Init(ADC1, &ADC_InitStructure);
  ADC_Calibration(ADC1);
  while (ADC_On(ADC1))
    delay(1000);

  TIM14->CNT = 0;
  TIM_Cmd(TIM14, ENABLE);
  NVIC_Init(TIM14_IRQn, 3, 1, ENABLE);
}

int main(void)
{
	char c;
	
	serial_handler = NULL;
  timer_event = 0;
	write_buffer_p = read_buffer_p = send_buffer;
	
	SystemPostInit();

	drill_init();
	heater_init();
	
  serial_init(SERIAL_CONSOLE, 115200, serial_receive, NULL);

  if (RCC->CSR & RCC_CSR_LPWRRSTF)	
	  str_send("Low-power reset");
  if (RCC->CSR & RCC_CSR_WWDGRSTF)	
	  str_send("Window watchdog reset");
  if (RCC->CSR & RCC_CSR_IWDGRSTF)	
	  str_send("Independent watchdog reset");
  if (RCC->CSR & RCC_CSR_SFTRSTF)	
	  str_send("Software reset");
  if (RCC->CSR & RCC_CSR_PORRSTF)	
	  str_send("POR/PDR reset");
  if (RCC->CSR & RCC_CSR_PINRSTF)	
	  str_send("PIN reset");
  if (RCC->CSR & RCC_CSR_OBL)	
	  str_send("Option byte loader reset");
  if (RCC->CSR & RCC_CSR_V18PWRRSTF)	
	  str_send("Reset of the 1.8 V domain");

	RCC->CSR |= RCC_CSR_RMVF;
	
  while (1)
  {
		while (write_buffer_p > read_buffer_p)
		{
			c = *read_buffer_p++;
			if (c == 'I')
				str_send("DRILLIRONC1.0");
			else if (c == 's')
				heater_send_status();
			else
				char_send(c);
		}
		__disable_irq();
		if (write_buffer_p == read_buffer_p)
		{
    	write_buffer_p = read_buffer_p = send_buffer;
		}
		__enable_irq();
		if (timer_event)
		{
			timer_event = 0;
  		drill_timer_proc();
	  	heater_timer_proc();
		}
    __wfi(); // entering sleep mode
  }
}
