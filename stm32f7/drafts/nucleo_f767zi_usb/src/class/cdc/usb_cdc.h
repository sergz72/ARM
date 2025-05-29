#ifndef USB_CDC_H
#define USB_CDC_H

#include <usb_device.h>

void USB_CDC_Init(unsigned int device_id, unsigned int num_ports,
                  void (*rx_callback)(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length));

#endif
