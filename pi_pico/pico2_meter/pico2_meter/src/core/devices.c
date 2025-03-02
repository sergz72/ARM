#include "devices.h"
#include <string.h>
#include "board.h"
#include "device_list.h"
#include <24c01_16.h>
#include <generic_dds.h>
#include <stdlib.h>

#include "i2c_soft.h"

DeviceObject device_list[MAX_TOTAL_DEVICES];

static TransferFunction GetDeviceInterface(int idx)
{
  if (SDA_IN(idx))
    return i2c_transfer;
  init_spi(idx);
  return spi_transfer;
}

static void FindI2CDeviceIds(DeviceObject *o)
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
}

static void FindSPIDeviceIds(DeviceObject *o)
{
  unsigned char command = DEVICE_COMMAND_GET_ID;
  unsigned char ids[MAX_SUBDEVICES];

  if (o->transfer(o, &command, 1, ids, MAX_SUBDEVICES))
    return;
  for (int subdevice = 0; subdevice < MAX_SUBDEVICES; subdevice++)
  {
    unsigned char id = ids[subdevice];
    if (!id)
      break;
    const Device *d = devices;
    for (int i = 0; i < MAX_KNOWN_DEVICES; i++)
    {
      if (d->device_id == id)
      {
        d->initializer(o);
        if (o->device_config)
          o->device = d;
        break;
      }
      d++;
    }
    o++;
  }
}

static void FindDeviceIds(int idx)
{
  DeviceObject *o = &device_list[idx * MAX_SUBDEVICES];
  DeviceObject *oo = o;

  for (int subdevice = 0; subdevice < MAX_SUBDEVICES; subdevice++)
  {
    oo->device = NULL;
    if (oo->device_config)
    {
      free(oo->device_config);
      oo->device_config = NULL;
    }
    if (oo->device_data)
    {
      free(oo->device_data);
      oo->device_data = NULL;
    }
    oo++;
  }

  change_channel(idx);
  if (!o->transfer)
  {
    o->transfer = GetDeviceInterface(idx);
    oo = &o[1];

    for (int subdevice = 1; subdevice < MAX_SUBDEVICES; subdevice++)
    {
      oo->transfer = o->transfer;
      oo++;
    }
  }
  if (o->transfer == i2c_transfer)
    FindI2CDeviceIds(o);
  else
    FindSPIDeviceIds(o);
}

void InitDeviceLists(void)
{
  memset(device_list, 0, sizeof(device_list));
  for (int i = 0; i < MAX_TOTAL_DEVICES; i++)
  {
    device_list[i].idx = i / MAX_SUBDEVICES;
    device_list[i].subdevice = i % MAX_SUBDEVICES;
  }
}

void BuildDeviceList(void)
{
  int i;

  for (i = 0; i < MAX_DEVICES; i++)
    FindDeviceIds(i);
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
