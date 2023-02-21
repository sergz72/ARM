#include "dev_voltmeter.h"
#include "ui.h"
#include "settings.h"
#include "stddef.h"
#include <malloc.h>
#include "board.h"

#define VREF 330

typedef struct {
  int v[3];
} VoltmeterData;

void* voltmeter_initializer(void)
{
  return (void*)1;
}

void voltmeter_ui_init_handler(void* config)
{
  LED_ClearScreen();
}

static void getResult(int channel, int settingId, void* d)
{
  VoltmeterData* data;

  if (d)
  {
    data = (VoltmeterData*)d;
    data->v[channel] = (int)(adc_read() * settings[settingId] * VREF / ADC_MAX / U_DIV);
  }
}

void* voltmeter_data_collector(int step, void* config, void* prev_data)
{
  switch (step)
  {
  case 0:
    adc_start_conversion(0);
    break;
  case 3:
    prev_data = malloc(sizeof(VoltmeterData));
    getResult(0, SETTING_U0MUL, prev_data);
    adc_start_conversion(1);
    break;
  case 6:
    getResult(1, SETTING_U1MUL, prev_data);
    adc_start_conversion(2);
    break;
  case 9:
    getResult(2, SETTING_U2MUL, prev_data);
    break;
  }

  return prev_data;
}

void voltmeter_ui_handler(void* data, void* config)
{
  int i;
  VoltmeterData* d = (VoltmeterData*)data;

  for (i = 0; i < 3; i++)
    LED_Printf(i, 0x20,  "%6d%02u", d->v[i] / 100, abs(d->v[i] % 100));
}
