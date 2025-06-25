#ifndef CDC_CLASS_H
#define CDC_CLASS_H

#include <usb_device.h>

#define CDC_MAX_PORTS 7

typedef struct
{
  unsigned int control_endpoint;
  unsigned int data_endpoint;
  unsigned char *buffer;
  unsigned char *buffer_read_p, *buffer_write_p;
} CDCPort;

class USB_CDC_Class: public USB_Class
{
  private:
    USB_DeviceManager *manager;
    CDCPort cdc_ports[CDC_MAX_PORTS];
    unsigned int port_mapping[USB_MAX_ENDPOINTS];
    unsigned int num_ports;
    unsigned int buffer_length;

    int InterfaceDescriptorBuilder(unsigned int port_id);
public:
    USB_CDC_Class(USB_DeviceManager *_manager, unsigned int _buffer_length);
    int DescriptorBuilder(unsigned int _num_ports);
    void InitEndpoint(unsigned int endpoint) override;
    void PacketReceived(unsigned int endpoint, void *data, unsigned int length) override;
    void SetupInterface(USBDeviceRequest *request) override;
    void Send(unsigned int port_id, unsigned char * buffer, unsigned int buffer_length);
    unsigned int GetPendingData(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length);
};
#endif
