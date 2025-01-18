#include "devices.h"
#include "board.h"
#include "device_list.h"

Device *device_list[MAX_DEVICES];
void *device_data[MAX_DEVICES];
void *device_config[MAX_DEVICES];

static int FindDeviceId(int idx)
{
  int i;
  const Device* d;

  d = devices;
  for (i = 0; i < MAX_KNOWN_DEVICES; i++)
  {
    if (I2CCheck(idx, d->device_id))
    {
      if (!device_config[idx])
        device_config[idx] = d->initializer(idx, &device_data[idx]);
      return d->device_id;
    }
    d++;
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
  int i, id;

  for (i = 0; i < MAX_DEVICES; i++)
  {
    id = FindDeviceId(i);
    if (id != 0)
    {
      const Device *d = devices;
      for (int j = 0; j < MAX_KNOWN_DEVICES; j++)
      {
        if (d->device_id == id)
        {
          device_list[i] = d;
          break;
        }
        d++;
      }
    }
    else
      device_list[i] = NULL;
  }
}
