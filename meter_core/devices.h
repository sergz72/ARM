#ifndef _DEVICES_H
#define _DEVICES_H

#include "board.h"
#include "core_main.h"
#include <generic_meter.h>
#include <generic_pwm.h>

#define PIN_SDA 0
#define PIN_SCL 1

#define PIN_MOSI 0
#define PIN_SCK  1
#define PIN_MISO 2
#define PIN_NCS  3

struct _DeviceObject;

typedef struct {
  int device_id;
  int public_id;
  int (*is_allowed_for_module_id)(int module_id);
  void (*initializer)(struct _DeviceObject *object);
  int (*save_config)(struct _DeviceObject *object, void *buffer);
  int (*timer_event)(struct _DeviceObject *object, int step, unsigned char *buffer);
  int (*message_processor)(struct _DeviceObject *object, unsigned char *buffer, int len);
} Device;

typedef int (*TransferFunction)(struct _DeviceObject *o, const void *txdata, unsigned int txdatalength, void *rxdata,
                                unsigned int rxdatalength);

typedef struct _DeviceObject
{
  int idx;
  int subdevice;
  TransferFunction transfer;
  const Device *device;
  void *device_data;
  void *device_config;
} DeviceObject;

#define MAX_TOTAL_DEVICES MAX_DEVICES*MAX_SUBDEVICES

extern DeviceObject device_list[MAX_TOTAL_DEVICES];

void BuildDeviceList(void);
void InitDeviceLists(void);
int BuildMeterConfig(void *buffer, const MeterConfig *config, const char *name);
int BuildPowerMeterConfig(void *buffer, const PowerMeterConfig *config, const char *name);
int BuildPWMConfig(void *buffer, const PWMConfig *config, const char *name);
int AlwaysAllowed(int module_id);

#endif
