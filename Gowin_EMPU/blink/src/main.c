#include <gw1ns4c.h>
#include <gw1ns4c_gpio.h>
#include <gw1ns4c_timer.h>

#define SystemCoreClock 27000000

#define CYCLES_PER_MILLISEC (SystemCoreClock / 1000)

void initializeGPIO()
{
	GPIO_InitTypeDef gpioInitStruct;

	//Select pin0
	gpioInitStruct.GPIO_Pin = GPIO_Pin_0;

	//Set selected pins as output (see GPIOMode_TypeDef in gw1ns4c_gpio.h)
	gpioInitStruct.GPIO_Mode = GPIO_Mode_OUT;

	//Disable interrupts on selected pins (see GPIOInt_TypeDef)
	gpioInitStruct.GPIO_Int = GPIO_Int_Disable;

	//Initialize the GPIO using the configured init struct
	//GPIO0 is a pointer containing the memory address of the GPIO APB peripheral
	GPIO_Init(GPIO0, &gpioInitStruct);
}

void initializeTimer()
{
	TIMER_InitTypeDef timerInitStruct;

	timerInitStruct.Reload = 0;

	//Disable interrupt requests from timer for now
	timerInitStruct.TIMER_Int = DISABLE;

	//Disable timer enabling/clocking from external pins (GPIO)
	timerInitStruct.TIMER_Exti = TIMER_DISABLE;

	TIMER_Init(TIMER0, &timerInitStruct);
	TIMER_StopTimer(TIMER0);
}

void delayMillis(uint32_t ms)
{
	TIMER_StopTimer(TIMER0);
	//Reset timer just in case it was modified elsewhere
	TIMER_SetValue(TIMER0, 0);
	TIMER_EnableIRQ(TIMER0);

	uint32_t reloadVal = CYCLES_PER_MILLISEC * ms;
	//Timer interrupt will trigger when it reaches the reload value
	TIMER_SetReload(TIMER0, reloadVal);

	TIMER_StartTimer(TIMER0);
	//Block execution until timer wastes the calculated amount of cycles
	while (TIMER_GetIRQStatus(TIMER0) != SET)
		;

	TIMER_StopTimer(TIMER0);
	TIMER_ClearIRQ(TIMER0);
	TIMER_SetValue(TIMER0, 0);
}

void SystemInit(void)
{
	initializeGPIO();
	initializeTimer();
}

int main(void)
{
  while(1)
  {
  	GPIO_ResetBit(GPIO0, GPIO_Pin_0);
  	delayMillis(500);
  	GPIO_SetBit(GPIO0, GPIO_Pin_0);
  	delayMillis(500);
  }
}
