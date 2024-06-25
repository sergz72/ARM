#include "dev_voltmeter.h"
#include "ui.h"
#include "settings.h"
#include "stddef.h"
#include <malloc.h>
#include "board.h"
#include <stdlib.h>

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

static void getResult(int channel, int offsetId, int settingId, void* d)
{
  VoltmeterData* data;
  int result, offset;

  if (d)
  {
    data = (VoltmeterData*)d;
    result = adc_get_result();
    offset = settings[offsetId];
    if (result >= offset)
      result -= offset;
    else
      result = 0;
    data->v[channel] = (int)(result * settings[settingId] * VREF / ADC_MAX / U_DIV);
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
    getResult(0, SETTING_U0OFFSET, SETTING_U0MUL, prev_data);
    adc_start_conversion(1);
    break;
  case 6:
    getResult(1, SETTING_U1OFFSET, SETTING_U1MUL, prev_data);
    adc_start_conversion(2);
    break;
  case 9:
    getResult(2, SETTING_U2OFFSET, SETTING_U2MUL, prev_data);
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

static void offset_calibration(printf_func pfunc, int channel, int offsetId, int settingId)
{
  VoltmeterData data;
  long long int offset = settings[offsetId];
  pfunc("calibrating channel %d\n", channel);
  while (settings[offsetId])
  {
    adc_start_conversion(channel);
    adc_wait();
    getResult(channel, offsetId, settingId, &data);
    pfunc("offset %lld result %d\n", settings[offsetId], data.v[channel]);
    if (!data.v[channel])
      settings[offsetId]--;
    else
      break;
  }
  if (!settings[offsetId])
  {
    settings[offsetId] = offset;
    pfunc("channel %d calibration failed.\n", channel);
    return;
  }
  while (settings[offsetId] < 1000)
  {
    adc_start_conversion(channel);
    adc_wait();
    getResult(channel, offsetId, settingId, &data);
    pfunc("offset %lld result %d\n", settings[offsetId], data.v[channel]);
    if (data.v[channel])
      settings[offsetId]++;
    else
      break;
  }
  if (!settings[offsetId])
  {
    settings[offsetId] = offset;
    pfunc("channel %d calibration failed.\n", channel);
    return;
  }
  pfunc("channel %d offset calibration finished. Offset = %lld\n", channel, settings[offsetId]);
}

static void coef_calibration(printf_func pfunc, int channel, int offsetId, int settingId, int mv)
{
  VoltmeterData data;
  long long int coef = settings[settingId];
  long long int above;
  pfunc("calibrating channel %d\n", channel);
  mv /= 10;
  while (settings[settingId] < 7000)
  {
    adc_start_conversion(channel);
    adc_wait();
    getResult(channel, offsetId, settingId, &data);
    pfunc("coef %lld result %d\n", settings[settingId], data.v[channel]);
    if (data.v[channel] <= mv)
      settings[settingId]++;
    else
      break;
  }
  if (settings[settingId] == 7000)
  {
    settings[settingId] = coef;
    pfunc("channel %d calibration failed.\n", channel);
    return;
  }
  above = settings[settingId];
  while (settings[settingId])
  {
    settings[settingId]--;
    adc_start_conversion(channel);
    adc_wait();
    getResult(channel, offsetId, settingId, &data);
    pfunc("coef %lld result %d\n", settings[settingId], data.v[channel]);
    if (data.v[channel] > mv)
      above = settings[settingId];
    else if (data.v[channel] < mv)
      break;
  }
  if (!settings[settingId])
  {
    settings[settingId] = coef;
    pfunc("channel %d calibration failed.\n", channel);
    return;
  }
  settings[settingId] = (above + settings[settingId]) / 2;
  pfunc("channel %d coef calibration finished. Coef = %lld\n", channel, settings[settingId]);
}

int voltmeter_calibrate(printf_func pfunc, int argc, char** argv, void* device_config)
{
  int v;

  v = atoi(argv[0]);
  if (v < 0)
  {
    pfunc("Invalid value\n");
    return 1;
  }
  if (!v)
  {
    pfunc("offset calibration...\n");
    offset_calibration(pfunc, 0, SETTING_U0OFFSET, SETTING_U0MUL);
    offset_calibration(pfunc, 1, SETTING_U1OFFSET, SETTING_U1MUL);
    offset_calibration(pfunc, 2, SETTING_U2OFFSET, SETTING_U2MUL);
  }
  else
  {
    pfunc("coef calibration...\n");
    coef_calibration(pfunc, 0, SETTING_U0OFFSET, SETTING_U0MUL, v);
    coef_calibration(pfunc, 1, SETTING_U1OFFSET, SETTING_U1MUL, v);
    coef_calibration(pfunc, 2, SETTING_U2OFFSET, SETTING_U2MUL, v);
  }
  pfunc("calibration finished.\n");
  return 0;
}
