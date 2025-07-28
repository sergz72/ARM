#include "board.h"
#include <ui.h>
#include <lcd_ssd1357.h>
#include <spi_lcd_common.h>
#include <display2.h>

static int counter;

void DrawChar(unsigned int x, unsigned int y, unsigned int c, unsigned int text_color, unsigned int bk_color)
{
  LcdSetWindow(x, y, x + 7, y + 7);
  Lcd_SendCommand(SSD1357_CMD_WRITERAM);
  font8_symbols[c](text_color, bk_color);
}

static void init_row(unsigned int row, unsigned int from, unsigned int count, unsigned short color)
{
  Character c;

  c.x = from * 8;
  c.y = row * 8;
  c.textColor = color;
  c.bkColor = BLACK_COLOR;
  for (unsigned int i = 0; i < count; i++)
  {
    DisplayInitChar(i + from, row, &c);
    c.x += 8;
  }
}

void UI_Init(void)
{
  Rectangle r;

  counter = 0;

  UI_CommonInit();

  LcdInit(SSD1357_MADCTL_VALUE);
  DisplayInit();
  init_row(0, 0, 8, HIGH_COLOR);
  init_row(1, 0, 8, Z_COLOR);
  init_row(2, 0, 8, LOW_COLOR);
  init_row(3, 0, 6, HIGH_COLOR);
  DisplaySetChar(0, 3, CHAR_H);
  DisplaySetChar(4, 3, CHAR_PERCENT);
  init_row(4, 0, 6, Z_COLOR);
  DisplaySetChar(0, 4, CHAR_Z);
  DisplaySetChar(4, 4, CHAR_PERCENT);
  init_row(5, 0, 6, LOW_COLOR);
  DisplaySetChar(0, 5, CHAR_L);
  DisplaySetChar(4, 5, CHAR_PERCENT);
  init_row(6, 0, 3, HIGH_COLOR);
  DisplaySetChar(0, 6, CHAR_H);
  init_row(6, 3, 3, LOW_COLOR);
  DisplaySetChar(3, 6, CHAR_L);
  init_row(7, 0, 6, WHITE_COLOR);
  DisplaySetChar(0, 7, CHAR_U);
  Lcd_CS_High();

  r.x = 6 * 8;
  r.y = 3 * 8;
  r.width = 16;
  r.height = 10;
  DisplayInitRectangle(0, &r);
  r.y += 10;
  DisplayInitRectangle(1, &r);
  r.y += 10;
  DisplayInitRectangle(2, &r);
  r.y += 10;
  DisplayInitRectangle(3, &r);
}

void RAMFUNC DrawMode(void)
{
}

static void ShowVoltmeter(unsigned int voltage)
{
}

static void ShowLedData(void)
{
  DisplaySetRectangleColor(0, RGB(led_data[0].red, 0, 0));
  DisplaySetRectangleColor(1, RGB(led_data[1].red, led_data[1].green, 0));
  DisplaySetRectangleColor(2, RGB(0, led_data[2].green, 0));
  DisplaySetRectangleColor(3, RGB(0, 0, led_data[3].blue));
}

static void ShowFrequency(unsigned int row, unsigned long frequency)
{
  for (int i = 7; i >= 0; i--)
  {
    unsigned int c;
    if (i < 2 && !frequency)
      c = CHAR_SPACE;
    else
    {
      c = frequency % 10;
      frequency /= 10;
    }
    DisplaySetChar(i, row, i == 5 || i == 2 ? c + 10 : c);
  }
}

static void ShowDuty(unsigned int row, unsigned int duty)
{
  char c3 = duty / 100;
  if (!c3)
    c3 = CHAR_SPACE;
  DisplaySetChar(1, row, c3);
  char c = (duty / 10) % 10;
  if (c3 == CHAR_SPACE && !c)
    c = CHAR_SPACE;
  DisplaySetChar(2, row, c);
  c = duty % 10;
  DisplaySetChar(3, row, c);
}

static void ShowVoltage(unsigned int column, unsigned int value)
{
  char c = value / 1000 + 10;
  DisplaySetChar(column, 6, c);
  c = (value / 100) % 10;
  DisplaySetChar(column + 1, 6, c);
}

void Process_Timer_Event(void)
{
  calculate_led_data_with_rs();
  ShowLedData();
  Process_Button_Events();

  counter++;
  if (counter == 5)
  {
    counter = 0;
    ShowVoltmeter(get_adc_voltage());
    adc_start();
    ShowFrequency(0, counter_freq_high);
    ShowFrequency(1, counter_freq_rs);
    ShowFrequency(2, counter_freq_low);
    ShowDuty(3, counter_high * 100 / COUNTERS_MAX);
    ShowDuty(4, counter_z * 100 / COUNTERS_MAX);
    ShowDuty(5, counter_low * 100 / COUNTERS_MAX);
    if (uh_changed_to)
    {
      ShowVoltage(1, uh_changed_to);
      uh_changed_to = 0;
    }
    if (ul_changed_to)
    {
      ShowVoltage(4, ul_changed_to);
      ul_changed_to = 0;
    }
  }

  Lcd_CS_High();
}
