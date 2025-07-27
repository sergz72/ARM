#include "board.h"
#include <ui.h>
#include <lcd_st7789.h>
#include <spi_lcd_common.h>
#include <display.h>
#include <fonts/font16.h>

#define FONT courierNew16ptFontInfo

#define FREQUENCY_COLUMNS 8

const unsigned int frequency_columns[FREQUENCY_COLUMNS] = { 2, 3, 4, 6, 7, 8, 10, 11 };

static int counter;

static void init_row(unsigned int row, unsigned short color)
{
  Character c;

  c.font = &FONT;
  c.x = 0;
  c.y = row * FONT.char_height;
  c.textColor = color;
  c.bkColor = BLACK_COLOR;
  for (unsigned int i = 0; i < DISPLAY_MAX_COLUMNS; i++)
  {
    DisplayInitChar(i, row, &c);
    c.x += 16;
  }
  DisplaySetChar(0, row, 'F');
  DisplaySetChar(5, row, '.');
  DisplaySetChar(9, row, '.');
  DisplaySetChar(12, row, '0');
}

static void initU(unsigned int column, char t, unsigned short color)
{
  Character c;

  c.font = &FONT;
  c.x = column * 16;
  c.y = FONT.char_height * 3;
  c.textColor = color;
  c.bkColor = BLACK_COLOR;
  DisplayInitChar(column, 3, &c);
  DisplaySetChar(column, 3, 'U');
  c.x += 16;
  column++;
  DisplayInitChar(column, 3, &c);
  DisplaySetChar(column, 3, t);
  c.x += 32;
  column += 2;
  DisplayInitChar(column, 3, &c);
  c.x += 16;
  column++;
  DisplayInitChar(column, 3, &c);
  DisplaySetChar(column, 3, '.');
  c.x += 16;
  column++;
  DisplayInitChar(column, 3, &c);
}

static void init_row3(void)
{
  initU(0, 'H', HIGH_COLOR);
  initU(7, 'L', LOW_COLOR);
}

static void initP(unsigned int column, unsigned int row, char t, unsigned short color)
{
  Character c;

  c.font = &FONT;
  c.x = column * 16;
  c.y = FONT.char_height * row;
  c.textColor = color;
  c.bkColor = BLACK_COLOR;
  DisplayInitChar(column, row, &c);
  DisplaySetChar(column, row, t);
  c.x += 16;
  column++;
  DisplayInitChar(column, row, &c);
  c.x += 16;
  column++;
  DisplayInitChar(column, row, &c);
  c.x += 16;
  column++;
  DisplayInitChar(column, row, &c);
  c.x += 16;
  column++;
  DisplayInitChar(column, row, &c);
  DisplaySetChar(column, row, '%');
}

static void init_row4(void)
{
  initP(0, 4, 'H', HIGH_COLOR);
  initP(5, 4, 'Z', Z_COLOR);
}

static void init_row5(void)
{
  initP(0, 5, 'L', LOW_COLOR);
}

void UI_Init(void)
{
  Rectangle r;

  counter = 0;

  UI_CommonInit();

  LcdInit(ST7789_MADCTL_VALUE);
  DisplayInit();
  init_row(0, HIGH_COLOR);
  init_row(1, Z_COLOR);
  init_row(2, LOW_COLOR);
  DisplaySetChar(1, 0, 'H');
  DisplaySetChar(1, 2, 'L');
  init_row3();
  init_row4();
  init_row5();

  r.x = 16 * 13;
  r.y = 0;
  r.width = 32;
  r.height = 33;
  DisplayInitRectangle(0, &r);
  r.y += 33;
  DisplayInitRectangle(1, &r);
  r.y += 33;
  DisplayInitRectangle(2, &r);
  r.y += 33;
  DisplayInitRectangle(3, &r);
}

void RAMFUNC DrawMode(void)
{
}

static void ShowVoltmeter(unsigned int voltage)
{
}

static void ShowFrequency(unsigned int row, unsigned int frequency)
{
  for (int i = FREQUENCY_COLUMNS - 1; i >= 0; i--)
  {
    char c;
    if (i < 2 && !frequency)
      c = ' ';
    else
    {
      c = (frequency % 10) + '0';
      frequency /= 10;
    }
    DisplaySetChar(frequency_columns[i], row, c);
  }
}

static void ShowDuty(unsigned int column, unsigned int row, unsigned int duty)
{
  char c3 = (duty / 100) + '0';
  if (c3 == '0')
    c3 = ' ';
  DisplaySetChar(column, row, c3);
  char c = ((duty / 10) % 10) + '0';
  if (c3 == ' ' && c == '0')
    c = ' ';
  column++;
  DisplaySetChar(column, row, c);
  c = (duty % 10) + '0';
  column++;
  DisplaySetChar(column, row, c);
}

static void ShowVoltage(unsigned int column, unsigned int value)
{
  char c = (value / 1000) + '0';
  DisplaySetChar(column, 3, c);
  c = ((value / 100) % 10) + '0';
  DisplaySetChar(column + 2, 3, c);
}

static void ShowLedData(void)
{
  DisplaySetRectangleColor(0, RGB(led_data[0].red, 0, 0));
  DisplaySetRectangleColor(1, RGB(led_data[1].red, led_data[1].green, 0));
  DisplaySetRectangleColor(2, RGB(0, led_data[2].green, 0));
  DisplaySetRectangleColor(3, RGB(0, 0, led_data[3].blue));
}

static unsigned int calculate_duty_z(unsigned int duty_h, unsigned int duty_l)
{
  duty_h += duty_l;
  if (duty_h >= 100)
    return 0;
  return 100 - duty_h;
}

void Process_Timer_Event(void)
{
#ifdef WITH_RS
    calculate_led_data_with_rs();
#else
    calculate_led_data_without_rs();
#endif

  ShowLedData();

  Process_Button_Events();

  counter++;
  if (counter == 5)
  {
    counter = 0;
    if (voltmeter_mode)
    {
      ShowVoltmeter(get_adc_voltage());
      adc_start();
    }
    ShowFrequency(0, counter_freq_high);
#ifdef WITH_RS
      ShowFrequency(1, counter_freq_rs);
#endif
    ShowFrequency(2, counter_freq_low);
    unsigned int duty_h = counter_high * 100 / COUNTERS_MAX;
    unsigned int duty_l = counter_low * 100 / COUNTERS_MAX;
    ShowDuty(1, 4, duty_h);
#ifdef WITH_RS
    ShowDuty(6, 4, counter_z * 100 / COUNTERS_MAX);
#else
    ShowDuty(6, 4, calculate_duty_z(duty_h, duty_l));
#endif
    ShowDuty(1, 5, duty_l);
    if (uh_changed_to)
    {
      ShowVoltage(3, uh_changed_to);
      uh_changed_to = 0;
    }
    if (ul_changed_to)
    {
      ShowVoltage(10, ul_changed_to);
      ul_changed_to = 0;
    }
  }
}
