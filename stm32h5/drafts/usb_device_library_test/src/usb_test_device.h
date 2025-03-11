#ifndef USB_DEVICE_DRD_H
#define USB_DEVICE_DRD_H

#include <usb_device.h>

void USBTestDeviceInit(void);
int SendDescriptorRequest(USBDescriptorType type, unsigned short index, unsigned short length, unsigned char *buffer);

#endif
