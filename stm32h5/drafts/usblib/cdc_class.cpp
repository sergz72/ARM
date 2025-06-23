#include <cdc_class.h>
#include <cstring>

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
  .number_endpoints = 1,
  .interface_class = usb_class_cdc,
  .interface_subclass = 2,
  .interface_protocol = 1,
  .interface_name = "CDC control interface"
};

static const USBInterfaceDescriptor cdc_data_interface =
{
  .number_endpoints = 2,
  .interface_class = usb_class_cdc_data,
  .interface_subclass = 0,
  .interface_protocol = 0,
  .interface_name = "CDC data interface"
};

static const USBEndpointDescriptor cdc_control_endpoint =
{
  .endpoint_number_increment = 1,
  .in_endpoint = 1,
  .transfer_type = usb_endpoint_transfer_type_interrupt,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .interval = 0x10
};

static const USBEndpointDescriptor cdc_data_out_endpoint =
{
  .endpoint_number_increment = 0,
  .in_endpoint = 0,
  .transfer_type = usb_endpoint_transfer_type_bulk,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .interval = 0
};

static const USBEndpointDescriptor cdc_data_in_endpoint =
{
  .endpoint_number_increment = 1,
  .in_endpoint = 1,
  .transfer_type = usb_endpoint_transfer_type_bulk,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .interval = 0
};

int USB_CDC_Class::InterfaceDescriptorBuilder(unsigned int port_id)
{
  unsigned int interface_control = manager->AddInterfaceDescriptor(this, &cdc_control_interface);
  unsigned char interface_data = (unsigned char)(interface_control + 1);
  manager->AddClassInterfaceDescriptor(0, 0x10, 1);
  manager->AddClassInterfaceDescriptor(1, 0, interface_data);
  manager->AddClassInterfaceDescriptor4(2, 2);
  manager->AddClassInterfaceDescriptor(6, 0, interface_data);
  unsigned int ep = manager->AddEndpointDescriptor(this, &cdc_control_endpoint);
  if (ep == 0)
    return 1;
  cdc_ports[port_id].control_endpoint = ep;
  port_mapping[ep] = port_id;
  manager->AddInterfaceDescriptor(this, &cdc_data_interface);
  ep = manager->AddEndpointDescriptor(this, &cdc_data_out_endpoint);
  if (ep == 0)
    return 1;
  cdc_ports[port_id].data_endpoint = ep;
  port_mapping[ep] = port_id;
  manager->AddEndpointDescriptor(this, &cdc_data_in_endpoint);
  return 0;
}

int USB_CDC_Class::DescriptorBuilder(unsigned int num_ports)
{
  memset(cdc_ports, 0, sizeof(cdc_ports));
  memset(port_mapping, 0, sizeof(port_mapping));

  for (int i = 0; i < num_ports; i++)
  {
    if (num_ports > 1)
      manager->AddInterfaceAssociationDescriptor(&cdc_iad);
    if (InterfaceDescriptorBuilder(i))
      return 1;
  }
  return 0;
}

USB_CDC_Class::USB_CDC_Class(USB_DeviceManager *_manager, cdc_rx_callback_typedef rx_callback)
{
  manager = _manager;
  cdc_rx_callback = rx_callback;
}

