#ifndef USB_DEVICE_DRD_H
#define USB_DEVICE_DRD_H

#include <usb_device.h>

void USBDeviceDrdInit(void);
extern const char drd_endpoints[USB_MAX_ENDPOINTS];

#endif
