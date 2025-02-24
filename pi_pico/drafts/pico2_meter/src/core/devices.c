#include "devices.h"
#include <string.h>
#include "board.h"
#include "device_list.h"
#include <24c01_16.h>
#include <generic_dds.h>
#include <stdlib.h>

#include "i2c_soft.h"

DeviceObject device_list[MAX_DEVICES];

static TransferFunction GetDeviceInterface(int idx)
{
  if (SDA_IN(idx))
    return i2c_transfer;
  init_spi(idx);
  return spi_transfer;
}

static void FindI2CDeviceId(DeviceObject *o)
{
  int i;
  const Device *d;
  unsigned int d_id;

  d = devices;
  for (i = 0; i < MAX_KNOWN_DEVICES; i++)
  {
    if (d->device_id <= 255)
    {
      int rc = I2CCheck(o->idx, d->device_id);
      if (rc)
      {
        d->initializer(o);
        if (o->device_config)
          o->device = d;
        return;
      }
    }
    d++;
  }
  if (!_24C01_16_read(o->idx, _24C01_16_address(0), 0, (unsigned char*)&d_id, 4, I2C_TIMEOUT))
  {
    d = devices;
    for (i = 0; i < MAX_KNOWN_DEVICES; i++)
    {
      if (d->device_id > 255 && d->device_id == d_id)
      {
        d->initializer(o);
        if (o->device_config)
          o->device = d;
        return;
      }
      d++;
    }
  }
  o->device = NULL;
}

static void FindSPIDeviceId(DeviceObject *o)
{
  unsigned char command = DEVICE_COMMAND_GET_ID;
  unsigned char id;

  if (o->transfer(o->idx, o->device->device_id, &command, 1, &id, 1))
    return;
  const Device *d = devices;
  for (int i = 0; i < MAX_KNOWN_DEVICES; i++)
  {
    if (d->device_id == id)
    {
      d->initializer(o);
      if (o->device_config)
        o->device = d;
      return;
    }
    d++;
  }
}

static void FindDeviceId(int idx)
{
  DeviceObject *o = &device_list[idx];

  o->device = NULL;
  if (o->device_config)
  {
    free(o->device_config);
    o->device_config = NULL;
  }
  if (o->device_data)
  {
    free(o->device_data);
    o->device_data = NULL;
  }

  change_channel(idx);
  if (!o->transfer)
    o->transfer = GetDeviceInterface(idx);
  if (o->transfer == i2c_transfer)
    FindI2CDeviceId(o);
  else
    FindSPIDeviceId(o);
}

void InitDeviceLists(void)
{
  memset(device_list, 0, sizeof(device_list));
  for (int i = 1; i < MAX_DEVICES; i++)
    device_list[i].idx = i;
}

void BuildDeviceList(void)
{
  int i;

  for (i = 0; i < MAX_DEVICES; i++)
    FindDeviceId(i);
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
