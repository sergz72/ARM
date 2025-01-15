#include "devices.h"
#include "board.h"
#include "device_list.h"

int device_list[MAX_DEVICES];
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
      device_config[idx] = d->initializer(idx);
      return d->device_id;
    }
    d++;
  }
  return 0;
}

void BuildDeviceList(void)
{
  int i, id;

  for (i = 0; i < MAX_DEVICES; i++)
  {
    id = FindDeviceId(i);
    device_list[i] = id;
  }
}

void BuildDeviceData(int step)
{
  int i, j, id;
  const Device* d;

  for (i = 0; i < MAX_DEVICES; i++)
  {
    id = device_list[i];
    if (id)
    {
      d = devices;
      for (j = 0; j < MAX_KNOWN_DEVICES; j++)
      {
        if (d->device_id == id)
        {
          if (d->data_collector)
            device_data[i] = d->data_collector(i, step, device_config[i], device_data[i]);
          else
            device_data[i] = NULL;
        }
        d++;
      }
    }
    else
      device_data[i] = NULL;
  }
}
