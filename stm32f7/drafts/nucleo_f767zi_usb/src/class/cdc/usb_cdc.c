#include <string.h>
#include <class/cdc/usb_cdc.h>

#define CDC_MAX_PORTS 7

static void USB_CDC_ControlInterfaceHandler(void *data, unsigned int epno, void *parameters);
static void USB_CDC_DataInterfaceHandler(void *data, unsigned int epno, void *parameters);
static void USB_CDC_ControlEndpointHandler(void *data, unsigned int epno, void *parameters);
static void USB_CDC_DataEndpointHandler(void *data, unsigned int epno, void *parameters);

typedef struct
{
  int control_endpoint;
  int data_endpoint;
} CDCPort;

static const USBInterfaceAssociationDescriptor cdc_iad =
{
  .interface_count = 2,
  .function_class = 2,
  .function_subclass = 2,
  .function_protocol = 1,
  .function_name = NULL
};

static const USBInterfaceDescriptor cdc_control_interface =
{
  .interface_name = "CDC control interface",
  .interface_class = usb_class_cdc,
  .interface_subclass = 2,
  .interface_protocol = 1,
  .number_endpoints = 1,
  .handler = USB_CDC_ControlInterfaceHandler
};

static const USBInterfaceDescriptor cdc_data_interface =
{
  .interface_name = "CDC data interface",
  .interface_class = usb_class_cdc_data,
  .interface_subclass = 0,
  .interface_protocol = 0,
  .number_endpoints = 2,
  .handler = USB_CDC_DataInterfaceHandler
};

static const USBEndpointDescriptor cdc_control_endpoint =
{
  .in_endpoint = 1,
  .endpoint_number_increment = 1,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .transfer_type = usb_endpoint_transfer_type_interrupt,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .interval = 0x10,
  .handler = USB_CDC_ControlEndpointHandler
};

static const USBEndpointDescriptor cdc_data_out_endpoint =
{
  .in_endpoint = 0,
  .endpoint_number_increment = 0,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .transfer_type = usb_endpoint_transfer_type_bulk,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .interval = 0,
  .handler = USB_CDC_DataEndpointHandler
};

static const USBEndpointDescriptor cdc_data_in_endpoint =
{
  .in_endpoint = 1,
  .endpoint_number_increment = 1,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .transfer_type = usb_endpoint_transfer_type_bulk,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .interval = 0,
  .handler = USB_CDC_DataEndpointHandler
};

typedef struct
{
  void (*cdc_rx_callback)(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length);
  CDCPort cdc_ports[CDC_MAX_PORTS];
  char port_mapping[USB_MAX_ENDPOINTS];
} CDCDevice;

static CDCDevice cdc_devices[USB_MAX_DEVICES];

static int cdc_descriptor_builder(unsigned int device_id, unsigned int num_ports);

void USB_CDC_Init(unsigned int device_id, unsigned int num_ports,
                  void (*rx_callback)(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length))
{
  cdc_devices[device_id].cdc_rx_callback = rx_callback;
  cdc_descriptor_builder(device_id, num_ports);
}

static int cdc_interface_descriptor(unsigned int device_id, USBDevice *device, int port_id)
{
  unsigned int interface_control = AddInterfaceDescriptor(device, &cdc_control_interface, NULL);
  unsigned char interface_data = (unsigned char)(interface_control + 1);
  AddClassInterfaceDescriptor(device, 0, 0x10, 1);
  AddClassInterfaceDescriptor(device, 1, 0, interface_data);
  AddClassInterfaceDescriptor4(device, 2, 2);
  AddClassInterfaceDescriptor(device, 6, 0, interface_data);
  int ep = AddEndpointDescriptor(device, &cdc_control_endpoint, (void*)port_id);
  if (ep < 0)
    return 1;
  cdc_devices[device_id].cdc_ports[port_id].control_endpoint = ep;
  cdc_devices[device_id].port_mapping[ep] = (char)port_id;
  AddInterfaceDescriptor(device, &cdc_data_interface, NULL);
  ep = AddEndpointDescriptor(device, &cdc_data_out_endpoint, (void*)port_id);
  if (ep < 0)
    return 1;
  cdc_devices[device_id].cdc_ports[port_id].data_endpoint = ep;
  cdc_devices[device_id].port_mapping[ep] = (char)port_id;
  AddEndpointDescriptor(device, &cdc_data_in_endpoint, (void*)port_id);
  return 0;
}

static int cdc_descriptor_builder(unsigned int device_id, unsigned int num_ports)
{
  USBDevice *device = &usb_devices[device_id];
  memset(cdc_devices[device_id].cdc_ports, -1, sizeof(cdc_devices[device_id].cdc_ports));
  memset(cdc_devices[device_id].port_mapping, -1, sizeof(cdc_devices[device_id].port_mapping));

  for (int i = 0; i < num_ports; i++)
  {
    if (num_ports > 1)
      AddInterfaceAssociationDescriptor(device, &cdc_iad);
    if (cdc_interface_descriptor(device_id, device, i))
      return 1;
  }
  return 0;
}

static void USB_CDC_ControlInterfaceHandler(void *data, unsigned int epno, void *parameters)
{
  //todo
}

static void USB_CDC_DataInterfaceHandler(void *data, unsigned int epno, void *parameters)
{
  //todo
}

static void USB_CDC_ControlEndpointHandler(void *data, unsigned int epno, void *parameters)
{
  //todo
}

static void USB_CDC_DataEndpointHandler(void *data, unsigned int epno, void *parameters)
{
  //todo
}
