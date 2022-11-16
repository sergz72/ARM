#include <board.h>
#include <ts_calibrate_ui.h>

void ts_calibrate_ui(void)
{
  ts_calibrate_point p[TS_CALIBRATE_POINTS];
  ts_calibration_data dt;
  ts_point tp;
  char text[30];
  int i;

  p[0].screen.x = LCD_WIDTH/2;
  p[0].screen.y = LCD_HEIGHT/2;
  p[1].screen.x = LCD_WIDTH/8;
  p[1].screen.y = LCD_HEIGHT/8;
  p[2].screen.x = LCD_WIDTH*7/8;
  p[2].screen.y = LCD_HEIGHT*7/8;
  p[3].screen.x = LCD_WIDTH*7/8;
  p[3].screen.y = LCD_HEIGHT/8;
  p[4].screen.x = LCD_WIDTH/8;
  p[4].screen.y = LCD_HEIGHT*7/8;

  LcdCursorEnable(NULL, 0);

  LcdClearDC(NULL); // clear the screen
#ifdef LCD_BUFFERED
  LcdUpdateDC(NULL);
#endif

  for (i = 0; i < TS_CALIBRATE_POINTS; i++)
  {
    LcdRectFill(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3, white_color);
#ifdef LCD_BUFFERED
		LcdUpdateRect(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3);
#endif
    do
    {
      while (TS_NOT_TOUCHED) delay(50000); // delay 50 ms
      p[i].adc_val = ts_get_adc_val();
    } while (p[i].adc_val.x < 0 || p[i].adc_val.y < 0);
    LcdRectFill(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3, black_color);
#ifdef LCD_BUFFERED
		LcdUpdateRect(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3);
#endif
    while (TS_TOUCHED) delay(10000); // delay 10 ms
    delay(50000); // debouncing
  }

  ts_calibrate(p);

  TS_GET_CALIBRATION_DATA(&dt);

  LcdPutString(NULL, "TS calibrated...\r\n");

  LcdPrintf(NULL, "alpha_x = %f ", dt.alpha_x);
  LcdPrintf(NULL, "beta_x = %f ", dt.beta_x);
  LcdPrintf(NULL, "delta_x = %f\r\n", dt.delta_x);

  LcdPrintf(NULL, "alpha_y = %f ", dt.alpha_y);
  LcdPrintf(NULL, "beta_y = %f ", dt.beta_y);
  LcdPrintf(NULL, "delta_y = %f\r\n", dt.delta_y);
#ifdef LCD_BUFFERED
  LcdUpdateDC(NULL);
#endif
	
  while (TS_NOT_TOUCHED) delay(50000); // delay 50 ms

  while (TS_TOUCHED) delay(10000); // delay 10 ms
  delay(50000); // debouncing

  LcdClearDC(NULL); // clear the screen

  LcdPutString(NULL, "Start testing...");
#ifdef LCD_BUFFERED
  LcdUpdateDC(NULL);
#endif

  for (i = 0; i < TS_CALIBRATE_POINTS; i++)
  {
    LcdRectFill(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3, white_color);
#ifdef LCD_BUFFERED
		LcdUpdateRect(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3);
#endif
    do
    {
      while (TS_NOT_TOUCHED) delay(50000); // delay 50 ms
      tp = ts_get_touch_pos();
    } while (tp.x == -1 || tp.y == -1);
    LcdRectFill(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3, black_color);
#ifdef LCD_BUFFERED
		LcdUpdateRect(NULL, p[i].screen.x-1, p[i].screen.y-1, 3, 3);
#endif
    LcdRectFill(NULL, tp.x-1, tp.y-1, 3, 3, white_color);
#ifdef LCD_BUFFERED
		LcdUpdateRect(NULL, tp.x-1, tp.y-1, 3, 3);
#endif
    while (TS_TOUCHED) delay(10000); // delay 10 ms
    delay(50000); // debouncing
    itoa(tp.x, text, 10);
    LcdDrawText(NULL, 0, 100, "      ");
    LcdDrawText(NULL, 0, 100, text);
    itoa(tp.y, text, 10);
    LcdDrawText(NULL, 0, 108, "      ");
    LcdDrawText(NULL, 0, 108, text);
#ifdef LCD_BUFFERED
    LcdUpdateDC(NULL);
#endif
  }

/*  tp.x = -1;

  for (i = 0; i < 10; i++)
  {
    if (tp.x > 0)
      LcdRectFill(NULL, tp.x-1, tp.y-1, 3, 3, black_color);
    do
    {
      tp = ts_get_touch_pos();
    } while (tp.x < 0 || tp.y < 0);
    LcdRectFill(NULL, tp.x-1, tp.y-1, 3, 3, white_color);
    delay(200000); // delay 200 ms
    itoa(tp.x, text, 10);
    LcdDrawText(NULL, 0, 100, "      ");
    LcdDrawText(NULL, 0, 100, text);
    itoa(tp.y, text, 10);
    LcdDrawText(NULL, 0, 108, "      ");
    LcdDrawText(NULL, 0, 108, text);
  }*/

  LcdClearDC(NULL); // clear the screen
}

#ifdef TS_TEST
void ts_adc_test(void)
{
  ts_point tp;
  char text[30];

  LcdCursorEnable(NULL, 0);

  LcdClearDC(NULL); // clear the screen
#ifdef LCD_BUFFERED
  LcdUpdateDC(NULL);
#endif

  while (1)
  {
    tp = ts_get_adc_val();
    if (tp.x < 0)
      continue;
    itoa(tp.x, text, 10);
    LcdDrawText(NULL, 0, 100, "      ");
    LcdDrawText(NULL, 0, 100, text);
    itoa(tp.y, text, 10);
    LcdDrawText(NULL, 0, 108, "      ");
    LcdDrawText(NULL, 0, 108, text);
#ifdef LCD_BUFFERED
    LcdUpdateDC(NULL);
#endif
    delay(100000); // delay 100 ms
  }
}
#endif
