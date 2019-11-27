#include "board.h"
#include "adc_func.h"
#include <st7066.h>
#include <adc.h>

unsigned short battery_voltage;

void AdcInit(void)
{
  battery_voltage = 0;

  st7066_set_ddram_address(ST066_LINE3_ADDRESS);
  st7066_write_data('V');
  st7066_write_data('B');
  st7066_write_data('A');
  st7066_write_data('T');
  st7066_set_ddram_address(ST066_LINE3_ADDRESS + 9);
  st7066_write_data('m');
  st7066_write_data('V');
}

void AdcLoop(void)
{
  ADC_ChannelConfTypeDef sConfig;
  unsigned int v, v2;

  sConfig.Channel      = ADC_CHANNEL_VREFINT;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_16CYCLES;
  ADC_ConfigChannel(ADC1, &sConfig);
  ADC_StartConversion(ADC1);
  if (ADC_PollForConversion(ADC1, 0xFFFF))
    v = ADC_GetValue(ADC1);
  else
    v = 0;

  st7066_set_ddram_address(ST066_LINE3_ADDRESS + 5);
  if (v > 0)
  {
    sConfig.Channel      = ADC_CHANNEL_20;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_16CYCLES;
    ADC_ConfigChannel(ADC1, &sConfig);
    ADC_StartConversion(ADC1);
    if (ADC_PollForConversion(ADC1, 0xFFFF))
    {
      //vbat = v2 * VREFINT_MV / v * (r2+r1)/r2
      v2 = ADC_GetValue(ADC1);
      v = (v2 * VREFINT_MV / v) * (VBAT_R1 + VBAT_R2) / VBAT_R2;
    }
    else
      v = 10000;

    if (v < 10000)
    {
      battery_voltage = v;
      st7066_write_data((v / 1000) + '0');
      st7066_write_data(((v / 100) % 10) + '0');
      st7066_write_data(((v / 10) % 10) + '0');
      st7066_write_data((v % 10) + '0');
      return;
    }
  }
  st7066_write_data('E');
  st7066_write_data('r');
  st7066_write_data('r');
  st7066_write_data(' ');
}