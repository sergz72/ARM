#ifndef USB_CDC_H
#define USB_CDC_H

#include <usb_device.h>

int USB_CDC_Init(const char enabled_endpoints[USB_MAX_ENDPOINTS],
                 int ports_count, int self_powered, int remote_wakeup, unsigned int max_power,
                 void (*rx_callback)(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length));

#endif
