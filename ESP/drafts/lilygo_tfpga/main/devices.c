#include "devices.h"

#include <esp_log.h>

#include "board.h"
#include "device_list.h"

static const char *TAG = "devices";

Device *device_list[MAX_DEVICES];
void *device_data[MAX_DEVICES];
void *device_config[MAX_DEVICES];

static const Device* FindDeviceId(int idx)
{
  int i;
  const Device* d;

  d = devices;
  for (i = 0; i < MAX_KNOWN_DEVICES; i++)
  {
    int rc = I2CCheck(idx, d->device_id);
    //ESP_LOGI(TAG, "I2CCheck %d %d rc %d", idx, d->device_id, rc);
    if (rc)
    {
      if (!device_config[idx])
        device_config[idx] = d->initializer(idx, &device_data[idx]);
      return d;
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
