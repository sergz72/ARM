#include "devices.h"
#include <string.h>
#include "board.h"
#include "device_list.h"
#include <24c01_16.h>
#include <stdlib.h>

#include "i2c_soft.h"

DeviceObject device_list[MAX_DEVICES];

static int i2c_transfer(int idx, int address, const void *txdata, unsigned int txdatalength, void *rxdata,
                        unsigned int rxdatalength)
{
  return i2c_soft_command(idx, address, NULL, 0, txdata, txdatalength, rxdata, rxdatalength, I2C_TIMEOUT);
}

static int spi_transfer(int idx, int address, const void *txdata, unsigned int txdatalength, void *rxdata,
                        unsigned int rxdatalength)
{
  //todo
  return 1;
}

static void InitSPI(int idx)
{
  //todo
}

static TransferFunction GetDeviceInterface(int idx)
{
  if (SDA_IN(idx))
    return i2c_transfer;
  InitSPI(idx);
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
        if (!o->device_config)
          d->initializer(o);
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
        if (!o->device_config)
          d->initializer(o);
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
  unsigned char command = 0;
  unsigned char id;

  if (o->transfer(o->idx, o->device->device_id, &command, 1, &id, 1))
  {
    o->device = NULL;
    return;
  }
  const Device *d = devices;
  for (int i = 0; i < MAX_KNOWN_DEVICES; i++)
  {
    if (d->device_id == id)
    {
      if (!o->device_config)
        d->initializer(o);
      o->device = d;
      return;
    }
    d++;
  }
  o->device = NULL;
}

static void FindDeviceId(int idx)
{
  DeviceObject *o = &device_list[idx];

  change_channel(idx);
  if (!o->transfer)
    o->transfer = GetDeviceInterface(idx);
  if (o->transfer == i2c_transfer)
    FindI2CDeviceId(o);
  else
    FindSPIDeviceId(o);
  if (o->device == NULL)
  {
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
  }
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
