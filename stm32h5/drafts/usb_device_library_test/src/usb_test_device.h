#ifndef USB_DEVICE_DRD_H
#define USB_DEVICE_DRD_H

#include <usb_device.h>

void USBTestDeviceInit(void);
unsigned char *SendDescriptorRequest(USBDescriptorType type);

#endif
