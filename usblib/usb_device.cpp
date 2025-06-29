#include <cstring>
#include <usb_device.h>
#include <stdlib.h>

#define USB_EP_IN 0x80

USB_DeviceManager::USB_DeviceManager(const USBDeviceConfiguration *conf,
  const USBConfigurationDescriptor *configuration_descriptor, USB_Device *dev)
{
  device = dev;
  config = conf;
  next_string_id = 1;
  next_interface_id = 0;
  next_endpoint_id = GetNextEndpoint(0);
  next_string_ptr = device_configuration + sizeof(device_configuration);
  next_descriptor_ptr = device_configuration;
  total_length = 0;
  InitEndpoints();
  memset(string_length, 0, sizeof(string_length));
  BuildDeviceDescriptor();
  AddConfigurationDescriptor(configuration_descriptor);
  memset(interface_handlers, 0, sizeof(interface_handlers));
  set_address = 0;
  language_id_descriptor[0] = 4;
  language_id_descriptor[1] = 3;
  language_id_descriptor[2] = (unsigned char)config->language_id;
  language_id_descriptor[3] = (unsigned char)(config->language_id >> 8);
}

unsigned char USB_DeviceManager::BuildString(const char *str)
{
  if (!str)
    return 0;

  unsigned char length = 0;
  unsigned int l;

  while (*str)
  {
    str++;
    length++;
  }
  l = length;
  while (l--)
  {
    *--next_string_ptr = 0;
    *--next_string_ptr = *--str;
  }
  *--next_string_ptr = string_descriptor_type;
  length = length * 2 + 2;
  *--next_string_ptr = length;
  string_table[next_string_id - 1] = next_string_ptr;
  string_length[next_string_id - 1] = length;

  return next_string_id++;
}

void USB_DeviceManager::BuildDeviceDescriptor()
{
  unsigned char *ptr = device_descriptor;
  *ptr++ = 0x12; // length
  *ptr++ = device_descriptor_type;
  *ptr++ = 0; // version lo
  *ptr++ = 2; // version hi
  *ptr++ = config->device_class;
  *ptr++ = 2; // subclass
  *ptr++ = config->device_class == usb_class_misc ? 1 : 0; //protocol
  *ptr++ = USB_FS_MAX_PACKET_SIZE;
  *ptr++ = config->vendor_id & 0xFF;
  *ptr++ = config->vendor_id >> 8;
  *ptr++ = config->product_id & 0xFF;
  *ptr++ = config->product_id >> 8;
  *ptr++ = 0; // release lo
  *ptr++ = 2; // release hi
  *ptr++ = BuildString(config->manufacturer);
  *ptr++ = BuildString(config->product);
  *ptr++ = BuildString(config->serial_number);
  *ptr = 1; // one configuration
}

void USB_DeviceManager::InitEndpoints()
{
  memset((void*)endpoints, 0, sizeof(endpoints));
  endpoints[0].max_packet_length = USB_FS_MAX_PACKET_SIZE;
  endpoints[0].transfer_type = usb_endpoint_transfer_type_control;
  endpoints[0].endpoint_buffer = (unsigned char*)malloc(USB_FS_MAX_PACKET_SIZE);
  endpoints[0].direction = usb_endpoint_direction_inout;
  endpoints[0].handler = this;
}

unsigned int USB_DeviceManager::GetNextEndpoint(unsigned int endpoint_id)
{
  do
  {
    endpoint_id++;
    if (endpoint_id >= USB_MAX_ENDPOINTS)
      return 0;
  } while (!(device->GetEnabledEndpoints() & (1 << endpoint_id)));
  return endpoint_id;
}

void USB_DeviceManager::AddConfigurationDescriptor(const USBConfigurationDescriptor *configuration)
{
  *next_descriptor_ptr++ = 9; // length
  *next_descriptor_ptr++ = configuration_descriptor_type;
  total_length_ptr = next_descriptor_ptr;
  total_length = 9;
  *next_descriptor_ptr++ = 9;
  *next_descriptor_ptr++ = 0;
  num_interfaces_ptr = next_descriptor_ptr;
  *next_descriptor_ptr++ = 0; // initial num interfaces
  *next_descriptor_ptr++ = 1; // configuration id
  *next_descriptor_ptr++ = BuildString(configuration->configuration_name);
  unsigned char attributes = 0x80;
  device_status = 0;
  if (configuration->self_powered)
  {
    attributes |= 0x40;
    device_status |= 0x40;
  }
  if (configuration->remote_wakeup)
  {
    attributes |= 0x20;
    attributes |= 0x20;
    device_status |= 0x20;
  }
  *next_descriptor_ptr++ = attributes;
  unsigned char max_power = configuration->max_power >= 510 ? 255 : configuration->max_power / 2;
  *next_descriptor_ptr++ = max_power;
}

void USB_DeviceManager::UpdateTotalLength(unsigned short value)
{
  total_length += value;
  total_length_ptr[0] = total_length & 0xFF;
  total_length_ptr[1] = total_length >> 8;
}

void USB_DeviceManager::AddInterfaceAssociationDescriptor(const USBInterfaceAssociationDescriptor *interface_association)
{
  UpdateTotalLength(8);
  *next_descriptor_ptr++ = 8; // length
  *next_descriptor_ptr++ = interface_association_descriptor_type;
  *next_descriptor_ptr++ = next_interface_id;
  *next_descriptor_ptr++ = interface_association->interface_count;
  *next_descriptor_ptr++ = interface_association->function_class;
  *next_descriptor_ptr++ = interface_association->function_subclass;
  *next_descriptor_ptr++ = interface_association->function_protocol;
  *next_descriptor_ptr++ = BuildString(interface_association->function_name);
}

void USB_DeviceManager::AddClassInterfaceDescriptor(unsigned char subtype, unsigned char data1, unsigned char data2)
{
  UpdateTotalLength(5);
  *next_descriptor_ptr++ = 5; // length
  *next_descriptor_ptr++ = cs_interface_descriptor_type;
  *next_descriptor_ptr++ = subtype;
  *next_descriptor_ptr++ = data1;
  *next_descriptor_ptr++ = data2;
}

void USB_DeviceManager::AddClassInterfaceDescriptor4(unsigned char subtype, unsigned char data1)
{
  UpdateTotalLength(4);
  *next_descriptor_ptr++ = 4; // length
  *next_descriptor_ptr++ = cs_interface_descriptor_type;
  *next_descriptor_ptr++ = subtype;
  *next_descriptor_ptr++ = data1;
}

unsigned int USB_DeviceManager::AddInterfaceDescriptor(USB_Class *handler, const USBInterfaceDescriptor *interface)
{
  UpdateTotalLength(9);
  *next_descriptor_ptr++ = 9; // length
  *next_descriptor_ptr++ = interface_descriptor_type;
  *next_descriptor_ptr++ = next_interface_id;
  *next_descriptor_ptr++ = 0; //alternate setting
  *next_descriptor_ptr++ = interface->number_endpoints;
  *next_descriptor_ptr++ = interface->interface_class;
  *next_descriptor_ptr++ = interface->interface_subclass;
  *next_descriptor_ptr++ = interface->interface_protocol;
  *next_descriptor_ptr++ = BuildString(interface->interface_name);
  *num_interfaces_ptr = (unsigned char)next_interface_id + 1;
  interface_handlers[next_interface_id] = handler;
  return next_interface_id++;
}

void USB_DeviceManager::AddEndpointDescriptor(const USBEndpointDescriptor *endpoint, unsigned char flag)
{
  UpdateTotalLength(7);
  *next_descriptor_ptr++ = 7; // length
  *next_descriptor_ptr++ = endpoint_descriptor_type;
  *next_descriptor_ptr++ = next_endpoint_id | flag;
  unsigned char attributes = endpoint->transfer_type | (endpoint->synchronization_type << 2) | (endpoint->usage_type << 4);
  *next_descriptor_ptr++ = attributes;
  *next_descriptor_ptr++ = endpoint->max_packet_size & 0xFF;
  *next_descriptor_ptr++ = endpoint->max_packet_size >> 8;
  *next_descriptor_ptr++ = endpoint->interval;
}

unsigned int USB_DeviceManager::AddEndpointDescriptor(USB_Class *handler, const USBEndpointDescriptor *endpoint)
{
  if (next_endpoint_id == 0)
    return 0;

  switch (endpoint->direction)
  {
    case usb_endpoint_direction_in:
      AddEndpointDescriptor(endpoint, USB_EP_IN);
      break;
    case usb_endpoint_direction_out:
      AddEndpointDescriptor(endpoint, 0);
      break;
    case usb_endpoint_direction_inout:
      AddEndpointDescriptor(endpoint, USB_EP_IN);
      AddEndpointDescriptor(endpoint, 0);
      break;
    default:
      return 0;
  }
  unsigned int ep = next_endpoint_id;
  next_endpoint_id = GetNextEndpoint(next_endpoint_id);

  endpoints[ep].max_packet_length = endpoint->max_packet_size;
  endpoints[ep].transfer_type = endpoint->transfer_type;
  endpoints[ep].direction = endpoint->direction;
  endpoints[ep].handler = handler;
  endpoints[ep].endpoint_buffer = (unsigned char *)malloc(endpoint->max_packet_size);

  return ep;
}

int USB_DeviceManager::Init()
{
  if (next_descriptor_ptr >= next_string_ptr)
    return 1;
  device->Init(this);
  Reset();
  device->Connect();
  return 0;
}

void *USB_DeviceManager::GetDescriptor(USBDescriptorType type, unsigned int id, unsigned int *length)
{
  unsigned int l;
  switch (type)
  {
    case device_descriptor_type:
      *length = USB_DEVICE_DESCRIPTOR_SIZE;
      return device_descriptor;
    case configuration_descriptor_type:
      *length = total_length;
      return device_configuration;
    case string_descriptor_type:
      if (!id)
      {
        *length = 4;
        return language_id_descriptor;
      }
      if (id >= next_string_id)
        return nullptr;
      l = string_length[id - 1];
      if (!l)
        return nullptr;
      *length = l;
      return string_table[id - 1];
    default: return nullptr;
  }
}

int USB_DeviceManager::StartTransfer(unsigned int endpoint, const void *buffer, unsigned int length)
{
  if (endpoints[endpoint].transfer_type == usb_endpoint_transfer_type_bulk && endpoints[endpoint].pending_transfer)
    return 1;
  unsigned int l = length > USB_FS_MAX_PACKET_SIZE ? USB_FS_MAX_PACKET_SIZE : length;
  device->SetEndpointData(endpoint, buffer, l);
  endpoints[endpoint].pending_transfer = 1;
  length -= l;
  if (length)
  {
    endpoints[endpoint].transfer_buffer = (unsigned char*)buffer + l;
    endpoints[endpoint].transfer_length = length;
  }
  return 0;
}

int USB_DeviceManager::ContinueTransfer(unsigned int endpoint)
{
  if (endpoints[endpoint].transfer_type == usb_endpoint_transfer_type_bulk && !endpoints[endpoint].pending_transfer)
    return 0;
  unsigned int l = endpoints[endpoint].transfer_length;
  if (set_address && !l)
  {
    device->SetAddress(set_address);
    set_address = 0;
  }
  if (l > USB_FS_MAX_PACKET_SIZE)
    l = USB_FS_MAX_PACKET_SIZE;
  device->SetEndpointData(endpoint, endpoints[endpoint].transfer_buffer, l);
  if (!l)
  {
    endpoints[endpoint].pending_transfer = 0;
    return 0;
  }
  endpoints[endpoint].transfer_length -= l;
  endpoints[endpoint].transfer_buffer += l;
  return 1;
}

void USB_DeviceManager::DeviceRequestHandler(USBDeviceRequest *request)
{
  unsigned int length;
  switch ((USBRequestType)request->request)
  {
    case usb_request_type_get_descriptor:
    {
      void *descriptor = GetDescriptor((USBDescriptorType)(request->value >> 8), request->value & 0xFF, &length);
      if (descriptor)
      {
        StartTransfer(0, descriptor, request->length > length ? length : request->length);
        device->ConfigureEndpointRX(0, usb_endpoint_configuration_enabled);
      }
      else
        device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
      break;
    }
    case usb_request_type_set_address:
      set_address = request->value;
      device->ZeroTransfer(0);
      break;
    case usb_request_type_set_configuration:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_get_configuration:
    {
      unsigned char configuration_id = 1;
      StartTransfer(0, &configuration_id, 1);
      break;
    }
    case usb_request_type_clear_feature:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_set_feature:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_get_status:
      StartTransfer(0, &device_status, 2);
      break;
    default:
      device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
      break;
  }
}

void USB_DeviceManager::ClassInterfaceRequestHandler(USBDeviceRequest *request)
{
  unsigned short interface_no = request->index;
  if (interface_no >= next_interface_id)
  {
    device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
    return;
  }
  USB_Class *handler = interface_handlers[interface_no];
  if (!handler)
  {
    device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
    return;
  }
  handler->SetupInterface(request);
  device->ConfigureEndpointRX(0, usb_endpoint_configuration_enabled);
}

void USB_DeviceManager::InterfaceRequestHandler(USBDeviceRequest *request)
{
  unsigned short interface_no = request->index;
  if (interface_no >= next_interface_id)
  {
    device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
    return;
  }
  switch ((USBRequestType)request->request)
  {
    case usb_request_type_clear_feature:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_set_feature:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_set_interface:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_get_interface:
    {
      unsigned char interface_id = 0;
      StartTransfer(0, &interface_id, 1);
      break;
    }
    case usb_request_type_get_status:
    {
      //todo
      unsigned short status = 0;
      StartTransfer(0, &status, 2);
      break;
    }
    default:
      device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
      break;
  }
}

void USB_DeviceManager::EndpointRequestHandler(USBDeviceRequest *request)
{
  switch ((USBRequestType)request->request)
  {
    case usb_request_type_clear_feature:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_set_feature:
      device->ZeroTransfer(0);
      break;
    case usb_request_type_get_status:
    {
      //todo
      unsigned short status = 0;
      StartTransfer(0, &status, 2);
      break;
    }
    default:
      device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
      break;
  }
}

void USB_DeviceManager::SetupPacketReceived(void *data)
{
  USBDeviceRequest *request = (USBDeviceRequest *)data;
  switch (request->request_type & 0x60)
  {
    case 0: // standard
      StandardSetupPacketReceived(request);
      break;
    case 0x20: // class
      ClassSetupPacketReceived(request);
      break;
    case 0x40: // vendor
      VendorSetupPacketReceived(request);
      break;
    default:
      device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
  }
}

void USB_DeviceManager::StandardSetupPacketReceived(USBDeviceRequest *request)
{
  switch (request->request_type & 0x1F)
  {
    case usb_request_recipient_device:
      DeviceRequestHandler(request);
      break;
    case usb_request_recipient_interface:
      InterfaceRequestHandler(request);
      break;
    case usb_request_recipient_endpoint:
      EndpointRequestHandler(request);
      break;
    default:
      device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
  }
}

void USB_DeviceManager::ClassSetupPacketReceived(USBDeviceRequest *request)
{
  switch (request->request_type & 0x1F)
  {
    case usb_request_recipient_interface:
      ClassInterfaceRequestHandler(request);
      break;
    default:
      device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
  }
}

void USB_DeviceManager::VendorSetupPacketReceived(USBDeviceRequest *request)
{
  device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
}

void USB_DeviceManager::DataPacketReceived(unsigned int endpoint, void *data, unsigned int length)
{
  auto handler = endpoints[endpoint].handler;
  if (!handler)
  {
    device->ConfigureEndpoint(endpoint, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
    return;
  }
  handler->PacketReceived(endpoint, data, length);
}

void USB_DeviceManager::Reset()
{
  for (unsigned int i = 0; i < USB_MAX_ENDPOINTS; i++)
  {
    auto handler = endpoints[i].handler;
    if (handler)
      handler->InitEndpoint(i);
  }
  device->Reset();
  set_address = 0;
}

void USB_DeviceManager::Suspend()
{
}

void USB_DeviceManager::Resume()
{
}

void USB_DeviceManager::Sof()
{
  //todo
}

unsigned int USB_DeviceManager::GetEndpointMaxTransferSize(unsigned int endpoint_no) const
{
  return endpoints[endpoint_no].max_packet_length;
}

USBEndpointDirection USB_DeviceManager::GetEndpointDirection(unsigned int endpoint_no) const
{
  return endpoints[endpoint_no].direction;
}

void USB_DeviceManager::InitEndpoint(unsigned int endpoint)
{
  device->SetEndpointTransferType(endpoint, usb_endpoint_transfer_type_control);
  device->ConfigureEndpoint(endpoint, usb_endpoint_configuration_enabled, usb_endpoint_configuration_stall);
}

USB_Device *USB_DeviceManager::GetDevice()
{
  return device;
}

void USB_DeviceManager::PacketReceived(unsigned int endpoint, void *data, unsigned int length)
{
  device->ConfigureEndpointRX(endpoint, usb_endpoint_configuration_enabled);
}

void USB_DeviceManager::SetupInterface(USBDeviceRequest *request)
{
  device->ConfigureEndpoint(0, usb_endpoint_configuration_stall, usb_endpoint_configuration_stall);
}