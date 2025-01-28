#include "devices.h"
#include <string.h>
#include "board.h"
#include "device_list.h"
#include <24c01_16.h>

Device *device_list[MAX_DEVICES];
void *device_data[MAX_DEVICES];
void *device_config[MAX_DEVICES];

static const Device* FindDeviceId(int idx)
{
  int i;
  const Device* d;
  unsigned int d_id;

  d = devices;
  change_channel(idx);
  for (i = 0; i < MAX_KNOWN_DEVICES; i++)
  {
    if (d->device_id <= 255)
    {
      int rc = I2CCheck(idx, d->device_id);
      if (rc)
      {
        if (!device_config[idx])
          device_config[idx] = d->initializer(idx, &device_data[idx]);
        return d;
      }
    }
    d++;
  }
  int rc = I2CCheck(idx, _24C01_16_address(0));
  if (rc && !_24C01_16_read(idx, _24C01_16_address(0), 0, (unsigned char*)&d_id, 4, I2C_TIMEOUT))
  {
    for (i = 0; i < MAX_KNOWN_DEVICES; i++)
    {
      if (d->device_id > 255 && d->device_id == d_id)
      {
        if (!device_config[idx])
          device_config[idx] = d->initializer(idx, &device_data[idx]);
        return d;
      }
    }
  }
  return 0;
}

void InitDeviceLists(void)
{
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    device_list[i] = NULL;
    device_data[i] = NULL;
    device_config[i] = NULL;
  }
}

void BuildDeviceList(void)
{
  int i;
  const Device *d;

  for (i = 0; i < MAX_DEVICES; i++)
  {
    d = FindDeviceId(i);
    if (d)
      device_list[i] = d;
    else
      device_list[i] = NULL;
  }
}

int BuildMeterConfig(void *buffer, const MeterConfig *config, const char *name)
{
  memcpy(buffer, config, sizeof(MeterConfig));
  int l = strlen(name);
  memcpy(buffer + sizeof(MeterConfig), name, l);
  return l + sizeof(MeterConfig);
}

int BuildPowerMeterConfig(void *buffer, const PowerMeterConfig *config, const char *name)
{
  memcpy(buffer, config, sizeof(PowerMeterConfig));
  int l = strlen(name);
  memcpy(buffer + sizeof(PowerMeterConfig), name, l);
  return l + sizeof(PowerMeterConfig);
}

int BuildPWMConfig(void *buffer, const PWMConfig *config, const char *name)
{
  memcpy(buffer, config, sizeof(PWMConfig));
  int l = strlen(name);
  memcpy(buffer + sizeof(PWMConfig), name, l);
  return l + sizeof(PWMConfig);
}
