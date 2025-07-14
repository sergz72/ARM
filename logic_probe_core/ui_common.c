#include "board.h"
#include <ui.h>
#include <string.h>

#define MAX_UH_VOLTAGES 5
#define MAX_UL_VOLTAGES 5

static const unsigned int uh_voltages[MAX_UH_VOLTAGES] = {
  2500,
  2000,
  1500,
  1000,
  880
};

static const unsigned int ul_voltages[MAX_UL_VOLTAGES] = {
  800,
  400,
  300,
  200,
  100
};

static int uh_index, ul_index;
static int prev_button1_pressed, prev_button2_pressed;
static int button1_pressed, button2_pressed;
unsigned int uh_changed_to, ul_changed_to;
int voltmeter_mode;
ws2812_rgb led_data[WS2812_MAX_LEDS];

void UI_CommonInit(void)
{
  memset(led_data, 0, sizeof(led_data));
  uh_index = 0;
  ul_index = 1;
  uh_changed_to = DEFAULT_DACH_VOLTAGE;
  ul_changed_to = DEFAULT_DACL_VOLTAGE;
  prev_button1_pressed = prev_button2_pressed = 0;
  button1_pressed = button2_pressed = voltmeter_mode = 0;
}

static void button1_short_press(void)
{
  uh_index++;
  if (uh_index >= MAX_UH_VOLTAGES)
    uh_index = 0;
  uh_changed_to = uh_voltages[uh_index];
  set_h_voltage(uh_changed_to);
}

static void button1_long_press(void)
{
  voltmeter_mode = !voltmeter_mode;
  if (voltmeter_mode)
    disconnect_pullup();
  else
    connect_pullup();
  DrawMode();
}

static void button2_short_press(void)
{
  ul_index++;
  if (ul_index >= MAX_UL_VOLTAGES)
    ul_index = 0;
  ul_changed_to = ul_voltages[ul_index];
  set_l_voltage(ul_changed_to);
}

static void button2_long_press(void)
{
}

void Process_Button_Events(void)
{
  int pressed = BUTTON1_PRESSED;
  if (!pressed && prev_button1_pressed)
  {
    if (button1_pressed < 10)
      button1_short_press();
    else
      button1_long_press();
  }
  if (pressed)
    button1_pressed++;
  else
    button1_pressed = 0;
  prev_button1_pressed = pressed;
  pressed = BUTTON2_PRESSED;
  if (!pressed && prev_button2_pressed)
  {
    if (button2_pressed < 10)
      button2_short_press();
    else
      button2_long_press();
  }
  if (pressed)
    button2_pressed++;
  else
    button2_pressed = 0;
  prev_button2_pressed = pressed;
}