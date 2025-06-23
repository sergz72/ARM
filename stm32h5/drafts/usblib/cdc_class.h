#ifndef CDC_CLASS_H
#define CDC_CLASS_H

#include <usb_device.h>

#define CDC_MAX_PORTS 7

typedef struct
{
  unsigned int control_endpoint;
  unsigned int data_endpoint;
} CDCPort;

extern "C" typedef void (*cdc_rx_callback_typedef)(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length);

class USB_CDC_Class: public USB_Class
{
  private:
    USB_DeviceManager *manager;
    cdc_rx_callback_typedef cdc_rx_callback;
    CDCPort cdc_ports[CDC_MAX_PORTS];
    unsigned int port_mapping[USB_MAX_ENDPOINTS];

    int InterfaceDescriptorBuilder(unsigned int port_id);
public:
    USB_CDC_Class(USB_DeviceManager *_manager, cdc_rx_callback_typedef rx_callback);
    int DescriptorBuilder(unsigned int num_ports);
};
#endif
