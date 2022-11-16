#include <board.h>
#include <ts_func.h>
#include <stdlib.h>

ts_point ts_get_adc_val(void)
{
  ts_point p[3];

  TS_ADC_ON;
  
  delay(50000); // debouncing

  if (TS_NOT_TOUCHED)
  {
    p[0].x = p[0].y = -1;
    return p[0];
  }

  for (;;)
  {
    TS_IO_MEASURE_X;
    delay(100);
    p[0].x = TS_YM_ADC_GET;
    delay(100);
    p[1].x = TS_YM_ADC_GET;
    delay(100);
    p[2].x = TS_YM_ADC_GET;

    TS_IO_MEASURE_Y;
    delay(100);
    p[0].y = TS_XM_ADC_GET;
    delay(100);
    p[1].y = TS_XM_ADC_GET;
    delay(100);
    p[2].y = TS_XM_ADC_GET;

    if (abs(p[0].x - p[1].x) >= 10) continue; 
    if (abs(p[0].x - p[2].x) >= 10) continue; 
    if (abs(p[1].x - p[2].x) >= 10) continue; 
    if (abs(p[0].y - p[1].y) >= 10) continue; 
    if (abs(p[0].y - p[2].y) >= 10) continue; 
    if (abs(p[1].y - p[2].y) >= 10) continue; 
    break;
  }

  TS_IO_STANDBY;
  TS_ADC_OFF;

  p[0].x = (p[0].x + p[1].x + p[2].x) / 3;
  p[0].y = (p[0].y + p[1].y + p[2].y) / 3;

  return p[0];
}
