#include <string.h>
#include <class/cdc/usb_cdc.h>

#define CDC_MAX_PORTS 7

typedef struct
{
  int control_endpoint;
  int data_endpoint;
} CDCPort;

static int cdc_descriptor_builder(void);

static const USBDeviceConfiguration configuration =
{
  .device_class = usb_class_cdc,
  .vendor_id = 1155,
  .product_id =  22336,
  .manufacturer = "STMicroelectronics",
  .product = "STM32 Virtual ComPort",
  .serial_number = "00000000001A",
  .num_configurations = 1,
  .descriptor_builder = &cdc_descriptor_builder,
};

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
  .number_endpoints = 1
};

static const USBInterfaceDescriptor cdc_data_interface =
{
  .interface_name = "CDC data interface",
  .interface_class = usb_class_cdc_data,
  .interface_subclass = 0,
  .interface_protocol = 0,
  .number_endpoints = 2
};

static const USBEndpointDescriptor cdc_control_endpoint =
{
  .in_endpoint = 1,
  .endpoint_number_increment = 1,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .transfer_type = usb_endpoint_transfer_type_interrupt,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .interval = 0x10
};

static const USBEndpointDescriptor cdc_data_out_endpoint =
{
  .in_endpoint = 0,
  .endpoint_number_increment = 0,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .transfer_type = usb_endpoint_transfer_type_bulk,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .interval = 0
};

static const USBEndpointDescriptor cdc_data_in_endpoint =
{
  .in_endpoint = 1,
  .endpoint_number_increment = 1,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .transfer_type = usb_endpoint_transfer_type_bulk,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .interval = 0
};

static void (*cdc_rx_callback)(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length);
static USBConfigurationDescriptor configuration_descriptor;
static CDCPort cdc_ports[CDC_MAX_PORTS];
static char port_mapping[USB_MAX_ENDPOINTS];

int USB_CDC_Init(const char enabled_endpoints[USB_MAX_ENDPOINTS],
                  int ports_count, int self_powered, int remote_wakeup, unsigned int max_power,
                  void (*rx_callback)(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length))
{
  cdc_rx_callback = rx_callback;
  configuration_descriptor.configuration_name = "CDC Config";
  configuration_descriptor.max_power = max_power;
  configuration_descriptor.num_interfaces = ports_count * 2;
  configuration_descriptor.remote_wakeup = remote_wakeup;
  configuration_descriptor.self_powered = self_powered;
  return USBDeviceInit(&configuration, enabled_endpoints);
}

static int cdc_interface_descriptor(int port_id)
{
  unsigned int interface_control = AddInterfaceDescriptor(&cdc_control_interface);
  unsigned char interface_data = (unsigned char)(interface_control + 1);
  AddClassInterfaceDescriptor(0, 0x10, 1);
  AddClassInterfaceDescriptor(1, 0, interface_data);
  AddClassInterfaceDescriptor4(2, 2);
  AddClassInterfaceDescriptor(6, 0, interface_data);
  int ep = AddEndpointDescriptor(&cdc_control_endpoint);
  if (ep < 0)
    return 1;
  cdc_ports[port_id].control_endpoint = ep;
  port_mapping[ep] = (char)port_id;
  AddInterfaceDescriptor(&cdc_data_interface);
  ep = AddEndpointDescriptor(&cdc_data_out_endpoint);
  if (ep < 0)
    return 1;
  cdc_ports[port_id].data_endpoint = ep;
  port_mapping[ep] = (char)port_id;
  AddEndpointDescriptor(&cdc_data_in_endpoint);
  return 0;
}

static int cdc_descriptor_builder(void)
{
  memset(cdc_ports, -1, sizeof(cdc_ports));
  memset(port_mapping, -1, sizeof(port_mapping));

  AddConfigurationDescriptor(&configuration_descriptor);
  unsigned int num_ports = configuration_descriptor.num_interfaces / 2;
  for (int i = 0; i < num_ports; i++)
  {
    if (configuration_descriptor.num_interfaces > 2)
      AddInterfaceAssociationDescriptor(&cdc_iad);
    if (cdc_interface_descriptor(i))
      return 1;
  }
  return 0;
}

void InTransactionHandler(int endpoint)
{
  //todo
  USBStallEndpoint(endpoint);
}

void InterfaceRequestHandler(USBDeviceRequest *request)
{
  //todo
  USBStallEndpoint(0);
}
