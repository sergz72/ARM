#include "board.h"
#include <timer.h>
#include <adc.h>
#include <lcd_ssd1306.h>
#include <fonts/font8_2.h>
#include <fonts/font16.h>
#include <stdio.h>

unsigned int timer_counter = 0;
unsigned int initialized = FALSE;
unsigned int updated = FALSE;
unsigned int vcc = 0, swr = 0, pwr = 0;

void TIM14_IRQHandler(void)
{
	unsigned int _vcc, _swr, _pwr, vrev, vfwd, _updated = FALSE;

  if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET)
  {
		timer_counter++;
    ADC_ChannelConfig(ADC1, ADC_Channel_0, ADC_SampleTime_1_5Cycles);
    // Start ADC1 Software Conversion 
    ADC_StartOfConversion(ADC1);
    /* Test EOC flag */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    /* Get ADC1 converted data */
    _vcc = ADC_GetConversionValue(ADC1);

    ADC_ChannelConfig(ADC1, ADC_Channel_1, ADC_SampleTime_1_5Cycles);
    ADC_StartOfConversion(ADC1);

		_vcc = _vcc * VREF * VCC_COEF / 0xFFF / VCC_DIV;

    /* Test EOC flag */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    /* Get ADC1 converted data */
    vrev = ADC_GetConversionValue(ADC1);

    ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_1_5Cycles);
    ADC_StartOfConversion(ADC1);

		vrev = vrev * VREF * FWD_COEF / 0xFFF / FWD_DIV;

    /* Test EOC flag */
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    /* Get ADC1 converted data */
    vfwd = ADC_GetConversionValue(ADC1) * VREF * FWD_COEF / 0xFFF / FWD_DIV;

    if (vrev > vfwd)
			vrev = vfwd;
    _swr = vfwd - vrev;
		if (_swr)
			_swr = (vfwd + vrev) * 100 / _swr;
		
		_pwr = vfwd * vfwd / RL / PWR_COEF;
		
    if (vcc != _vcc)
		{
			vcc = _vcc;
			_updated = TRUE;
		}

    if (swr != _swr)
		{
			swr = _swr;
			_updated = TRUE;
		}

    if (pwr != _pwr)
		{
			pwr = _pwr;
			_updated = TRUE;
		}
		
		updated = _updated;
		
    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
  }
}

int main(void)
{
	unsigned int _vcc, _swr, _pwr, width, w;
	static char buffer[15];
	
  while (1)
  {
  	if (!initialized && timer_counter == 2)
		{
			initialized = TRUE;
			updated = TRUE;
			LcdInit(&courierNew8ptFontInfo);
		}
  	if (initialized && updated)
		{
			_vcc = vcc;
			_swr = swr;
			_pwr = pwr;
			updated = FALSE;
			LcdClearDC(NULL);
			sprintf(buffer, "%2d.%d", _vcc / 10, _vcc % 10);
			width = LcdDrawText(NULL, 0, 0, buffer); // VCC
			w = _swr > SWR_MAX_BAR ? SWR_MAX_BAR : _swr;
			w = (LCD_WIDTH - width) * w / SWR_MAX_BAR;
			if (w)
			  LcdRectFill(NULL, width, 0, w, 10, WHITE_COLOR); // SWR BAR
			LcdSetFont(NULL, &courierNew16ptFontInfo);
			if (_swr == 0 || _swr > SWR_MAX)
			  LcdDrawText(NULL, 0, 12, "S:HI"); // SWR
			else
			{
			  sprintf(buffer, "S:%2d.%02d", _swr / 100, _swr % 100);
			  LcdDrawText(NULL, 0, 12, buffer); // SWR
			}
			sprintf(buffer, "P:%2d.%02d", _pwr / 100, _pwr % 100);
			LcdDrawText(NULL, 0, 32, buffer); // PWR
			LcdSetFont(NULL, &courierNew8ptFontInfo);
			w = _pwr > PWR_MAX_BAR ? PWR_MAX_BAR : _pwr;
			w = LCD_WIDTH * w / PWR_MAX_BAR;
			if (w)
			  LcdRectFill(NULL, 0, 54, w, 10, WHITE_COLOR); // PWR BAR
			LCD_Update();
		}
		__wfi();
  }
}
