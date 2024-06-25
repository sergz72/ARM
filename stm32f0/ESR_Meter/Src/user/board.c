#include <board.h>
#include <main.h>
#include <lcd_ssd1306.h>
#include <fonts/font24.h>
#include <fonts/font12.h>

extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;
extern ADC_HandleTypeDef hadc;

unsigned int calibration_mode, maximum;

void user_init(void)
{
	calibration_mode = 0;
	maximum = VCC; // 3.3v
	
	TIM16->DIER = TIM_DIER_UIE;
	HAL_TIM_Base_Start(&htim16);
	HAL_TIM_Base_Start(&htim14);
	HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1); 
	
  LcdInit(&courierNew12ptFontInfo);
	LcdClearDC(NULL);
}

void user_loop(void)
{
	//__wfi();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	char buffer[10];
  unsigned int mv, r;
	
  HAL_ADC_Start(&hadc);
	if (HAL_ADC_PollForConversion(&hadc, 0xFFFFFFFF) == HAL_OK)
	{
		mv = HAL_ADC_GetValue(&hadc) * VCC / 4096;
		if (BUTTON_PRESSED)
		{
			if (!calibration_mode)
			{
				maximum = mv;
				calibration_mode = 1;
			}
		}
		else
  	  calibration_mode = 0;

    if (mv > maximum)
			maximum = mv;

    sprintf(buffer, "%04d", maximum);
 		LcdDrawText(NULL, 0, 0, buffer);
    sprintf(buffer, "%04d", mv);
 		LcdDrawText(NULL, 63, 0, buffer);
		
		LcdSetFont(NULL, &courierNew24ptFontInfo);
		if (mv)
		{
		  r = (maximum - mv) * RL / mv;
			if (r < 10000)
		    sprintf(buffer, "%d/%03d", r / 1000, r % 1000);
			else
		    sprintf(buffer, "%d/%02d", r / 1000, (r / 10) % 100);
  		LcdDrawText(NULL, 1, 24, buffer);
		}
		else
  		LcdDrawText(NULL, 1, 24, "/////");
		LcdSetFont(NULL, &courierNew12ptFontInfo);
		LCD_Update();
	}
}
