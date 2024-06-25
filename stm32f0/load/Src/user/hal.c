#include <board.h>

extern volatile int encoder_counter;

void ST7066_DATA_SET(unsigned int d)
{
	GPIOB->ODR = (GPIOB->ODR & 7) | (d & 0xF8);
	GPIOA->ODR = (GPIOA->ODR & 0xF8FF) | ((d & 7) << 8);
}

/*void st7066_delay(void)
{
	int i;
	
	for (i = 0; i < 40; i++);
}*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
	
	if (DT_STATE)
    encoder_counter++;
	else
 		encoder_counter--;
}
