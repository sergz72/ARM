#include <cdc_class.h>
#include <cstring>
#include <stdlib.h>

#define CDC_GET_LINE_CODING         0X21                                      /* This request allows the host to find out the currently configured line coding */
#define CDC_SET_LINE_CODING         0x20                                      /* Configures DTE rate, stop-bits, parity, and number-of-character */

#define DEF_UART_BAUDRATE       115200
#define DEF_UART_STOPBIT        0
#define DEF_UART_PARITY         0
#define DEF_UART_DATABIT        8

#define LINE_CODING_LENGTH 8

const unsigned char Com_Cfg[LINE_CODING_LENGTH] = {
  (uint8_t)(DEF_UART_BAUDRATE & 0xFF),
  (uint8_t)((DEF_UART_BAUDRATE >> 8) & 0xFF),
  (uint8_t)((DEF_UART_BAUDRATE >> 16) & 0xFF),
  (uint8_t)((DEF_UART_BAUDRATE >> 24) & 0xFF),
  DEF_UART_STOPBIT,
  DEF_UART_PARITY,
  DEF_UART_DATABIT
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
  .transfer_type = usb_endpoint_transfer_type_interrupt,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .interval = 0x10,
  .direction = usb_endpoint_direction_in
};

static const USBEndpointDescriptor cdc_data_endpoint =
{
  .transfer_type = usb_endpoint_transfer_type_bulk,
  .synchronization_type = usb_endpoint_synchronization_type_none,
  .usage_type = usb_endpoint_usage_type_data,
  .max_packet_size = USB_FS_MAX_PACKET_SIZE,
  .interval = 0,
  .direction = usb_endpoint_direction_inout
};

int USB_CDC_Class::InterfaceDescriptorBuilder(unsigned int port_id)
{
  unsigned int interface_control = manager->AddInterfaceDescriptor(this, &cdc_control_interface);
  unsigned char interface_data = (unsigned char)(interface_control + 1);
  manager->AddClassInterfaceDescriptor(0, 0x10, 1);
  manager->AddClassInterfaceDescriptor(1, 0, interface_data);
  manager->AddClassInterfaceDescriptor4(2, 2);
  manager->AddClassInterfaceDescriptor(6, interface_control, interface_data);
  unsigned int ep = manager->AddEndpointDescriptor(this, &cdc_control_endpoint);
  if (ep == 0)
    return 1;
  cdc_ports[port_id].control_endpoint = ep;
  port_mapping[ep] = port_id;
  manager->AddInterfaceDescriptor(this, &cdc_data_interface);
  ep = manager->AddEndpointDescriptor(this, &cdc_data_endpoint);
  if (ep == 0)
    return 1;
  cdc_ports[port_id].data_endpoint = ep;
  port_mapping[ep] = port_id;
  return 0;
}

int USB_CDC_Class::DescriptorBuilder(unsigned int _num_ports)
{
  num_ports = _num_ports;
  memset(cdc_ports, 0, sizeof(cdc_ports));
  memset(port_mapping, 0, sizeof(port_mapping));

  for (int i = 0; i < num_ports; i++)
  {
    if (num_ports > 1)
      manager->AddInterfaceAssociationDescriptor(&cdc_iad);
    if (InterfaceDescriptorBuilder(i))
      return 1;
    cdc_ports[i].buffer = cdc_ports[i].buffer_read_p = cdc_ports[i].buffer_write_p = (unsigned char*)malloc(buffer_length);
  }
  return 0;
}

USB_CDC_Class::USB_CDC_Class(USB_DeviceManager *_manager, unsigned int _buffer_length)
{
  manager = _manager;
  buffer_length = _buffer_length;
}

void USB_CDC_Class::InitEndpoint(unsigned int endpoint)
{
  for (unsigned int i = 0; i < num_ports; i++)
  {
    manager->GetDevice()->SetEndpointTransferType(cdc_ports[i].control_endpoint, usb_endpoint_transfer_type_interrupt);
    manager->GetDevice()->ConfigureEndpoint(cdc_ports[i].control_endpoint, usb_endpoint_configuration_enabled, usb_endpoint_configuration_nak);
    manager->GetDevice()->SetEndpointTransferType(cdc_ports[i].data_endpoint, usb_endpoint_transfer_type_bulk);
    manager->GetDevice()->ConfigureEndpoint(cdc_ports[i].data_endpoint, usb_endpoint_configuration_enabled, usb_endpoint_configuration_nak);
  }
}

void USB_CDC_Class::PacketReceived(unsigned int endpoint, void *data, unsigned int length)
{
  unsigned int port_id = port_mapping[endpoint];
  auto port = &cdc_ports[port_id];
  if (endpoint == port->data_endpoint)
  {
    unsigned char *d = (unsigned char*)data;
    while (length--)
    {
      if (port->buffer_write_p == port->buffer_read_p - 1)
        break;
      *port->buffer_write_p++ = *d++;
      if (port->buffer_write_p == port->buffer + buffer_length)
        port->buffer_write_p = port->buffer;
    }
    manager->GetDevice()->ConfigureEndpointRX(endpoint, usb_endpoint_configuration_enabled);
  }
}

void USB_CDC_Class::SetupInterface(USBDeviceRequest *request)
{
  switch (request->request)
  {
    case CDC_GET_LINE_CODING:
      manager->StartTransfer(0, &Com_Cfg, LINE_CODING_LENGTH);
      break;
    case CDC_SET_LINE_CODING:
      manager->GetDevice()->ZeroTransfer(0);
      break;
    default:
      manager->GetDevice()->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
      break;
  }
}

void USB_CDC_Class::Send(unsigned int port_id, unsigned char * buffer, unsigned int buffer_length)
{
  while (manager->StartTransfer(cdc_ports[port_id].data_endpoint, buffer, buffer_length))
    ;
}

unsigned int USB_CDC_Class::GetPendingData(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length)
{
  auto port = &cdc_ports[port_id];
  unsigned int length = 0;
  while (port->buffer_read_p != port->buffer_write_p)
  {
    *buffer++ = *port->buffer_read_p++;
    if (port->buffer_read_p == port->buffer + buffer_length)
      port->buffer_read_p = port->buffer;
    length++;
    buffer_length--;
    if (!buffer_length)
      break;
  }
  return length;
}