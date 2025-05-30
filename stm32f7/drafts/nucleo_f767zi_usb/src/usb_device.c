#include <board.h>
#include <string.h>
#include <usb_device.h>

#define USB_EP_IN 0x80

static unsigned int next_usb_device_id = 0;

USBDevice usb_devices[USB_MAX_DEVICES];

static unsigned char BuildString(USBDevice *device, const char *str)
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
    *--device->next_string_ptr = 0;
    *--device->next_string_ptr = *--str;
  }
  *--device->next_string_ptr = string_descriptor_type;
  length = length * 2 + 2;
  *--device->next_string_ptr = length;
  device->string_table[device->next_string_id - 1] = device->next_string_ptr;
  device->string_length[device->next_string_id - 1] = length;

  return device->next_string_id++;
}

static void BuildDeviceDescriptor(USBDevice *device)
{
  unsigned char *ptr = device->device_descriptor;
  *ptr++ = 0x12; // length
  *ptr++ = device_descriptor_type;
  *ptr++ = 0; // version lo
  *ptr++ = 2; // version hi
  *ptr++ = device->config->device_class;
  *ptr++ = 2; // subclass
  *ptr++ = device->config->device_class == usb_class_misc ? 1 : 0; //protocol
  *ptr++ = USB_FS_MAX_PACKET_SIZE;
  *ptr++ = device->config->vendor_id & 0xFF;
  *ptr++ = device->config->vendor_id >> 8;
  *ptr++ = device->config->product_id & 0xFF;
  *ptr++ = device->config->product_id >> 8;
  *ptr++ = 0; // release lo
  *ptr++ = 2; // release hi
  *ptr++ = BuildString(device, device->config->manufacturer);
  *ptr++ = BuildString(device, device->config->product);
  *ptr++ = BuildString(device, device->config->serial_number);
  *ptr = 1; // one configuration
}

static void InitEndpoints(USBDevice *device, const char enabled_endpoints[USB_MAX_ENDPOINTS])
{
  memset(device->endpoints, 0, sizeof(device->endpoints));
  for (int i = 0; i < USB_MAX_ENDPOINTS; i++)
    device->endpoints[i].enabled = (int)enabled_endpoints[i];
}

static int USBGetNextEndpoint(USBDevice *device, int endpoint_id)
{
  do
  {
    endpoint_id++;
    if (endpoint_id >= USB_MAX_ENDPOINTS)
      return -1;
  } while (!device->endpoints[endpoint_id].enabled);
  return endpoint_id;
}

static void AddConfigurationDescriptor(USBDevice *device, const USBConfigurationDescriptor *configuration)
{
  *device->next_descriptor_ptr++ = 9; // length
  *device->next_descriptor_ptr++ = configuration_descriptor_type;
  device->total_length_ptr = device->next_descriptor_ptr;
  device->total_length = 9;
  *device->next_descriptor_ptr++ = 9;
  *device->next_descriptor_ptr++ = 0;
  device->num_interfaces_ptr = device->next_descriptor_ptr;
  *device->next_descriptor_ptr++ = 0; // initial num interfaces
  *device->next_descriptor_ptr++ = 0; // configuration id
  *device->next_descriptor_ptr++ = BuildString(device, configuration->configuration_name);
  unsigned char attributes = 0x80;
  if (configuration->self_powered)
    attributes |= 0x40;
  if (configuration->remote_wakeup)
    attributes |= 0x20;
  *device->next_descriptor_ptr++ = attributes;
  unsigned char max_power = configuration->max_power >= 510 ? 255 : configuration->max_power / 2;
  *device->next_descriptor_ptr++ = max_power;
}

unsigned int USBDeviceInit(const USBDeviceConfiguration *config, USBConfigurationDescriptor *configuration_descriptor)
{
  USBDevice *device = &usb_devices[next_usb_device_id];
  device->config = config;
  InitEndpoints(device, config->enabled_endpoints);
  device->next_string_id = 1;
  device->next_interface_id = 0;
  device->next_endpoint_id = USBGetNextEndpoint(device, 0);
  device->next_string_ptr = device->device_configuration + sizeof(device->device_configuration);
  device->next_descriptor_ptr = device->device_configuration;
  device->data = config->data;
  device->total_length = 0;
  memset(device->string_length, 0, sizeof(device->string_length));
  BuildDeviceDescriptor(device);
  USBInitEndpoint(config->data, 0, USB_FS_MAX_PACKET_SIZE, usb_endpoint_transfer_type_control);
  AddConfigurationDescriptor(device, configuration_descriptor);
  memset(device->Reset_Callbacks, 0, sizeof(device->Reset_Callbacks));
  memset(device->SOF_Callbacks, 0, sizeof(device->SOF_Callbacks));
  memset(device->LPM_Callbacks, 0, sizeof(device->LPM_Callbacks));
  memset(device->endpoint_packet_lengths, 0, sizeof(device->endpoint_packet_lengths));
  device->endpoint_packet_lengths[0] = USB_FS_MAX_PACKET_SIZE;
  memset(device->interface_handlers, 0, sizeof(device->interface_handlers));
  memset(device->endpoint_handlers, 0, sizeof(device->endpoint_handlers));
  return next_usb_device_id++;
}

int USBDeviceValidate(unsigned int device_id)
{
  USBDevice *device = &usb_devices[device_id];
  return device->next_descriptor_ptr < device->next_string_ptr ? 0 : 1;
}

static void update_total_length(USBDevice *device, unsigned short value)
{
  device->total_length += value;
  device->total_length_ptr[0] = device->total_length & 0xFF;
  device->total_length_ptr[1] = device->total_length >> 8;
}

void AddInterfaceAssociationDescriptor(USBDevice *device, const USBInterfaceAssociationDescriptor *interface_association)
{
  update_total_length(device, 8);
  *device->next_descriptor_ptr++ = 8; // length
  *device->next_descriptor_ptr++ = interface_association_descriptor_type;
  *device->next_descriptor_ptr++ = device->next_interface_id;
  *device->next_descriptor_ptr++ = interface_association->interface_count;
  *device->next_descriptor_ptr++ = interface_association->function_class;
  *device->next_descriptor_ptr++ = interface_association->function_subclass;
  *device->next_descriptor_ptr++ = interface_association->function_protocol;
  *device->next_descriptor_ptr++ = BuildString(device, interface_association->function_name);
}

void AddClassInterfaceDescriptor(USBDevice *device, unsigned char subtype, unsigned char data1, unsigned char data2)
{
  update_total_length(device, 5);
  *device->next_descriptor_ptr++ = 5; // length
  *device->next_descriptor_ptr++ = cs_interface_descriptor_type;
  *device->next_descriptor_ptr++ = subtype;
  *device->next_descriptor_ptr++ = data1;
  *device->next_descriptor_ptr++ = data2;
}

void AddClassInterfaceDescriptor4(USBDevice *device, unsigned char subtype, unsigned char data1)
{
  update_total_length(device, 4);
  *device->next_descriptor_ptr++ = 4; // length
  *device->next_descriptor_ptr++ = cs_interface_descriptor_type;
  *device->next_descriptor_ptr++ = subtype;
  *device->next_descriptor_ptr++ = data1;
}

unsigned int AddInterfaceDescriptor(USBDevice *device, const USBInterfaceDescriptor *interface, void *data)
{
  update_total_length(device, 9);
  *device->next_descriptor_ptr++ = 9; // length
  *device->next_descriptor_ptr++ = interface_descriptor_type;
  *device->next_descriptor_ptr++ = device->next_interface_id;
  *device->next_descriptor_ptr++ = 0; //alternate setting
  *device->next_descriptor_ptr++ = interface->number_endpoints;
  *device->next_descriptor_ptr++ = interface->interface_class;
  *device->next_descriptor_ptr++ = interface->interface_subclass;
  *device->next_descriptor_ptr++ = interface->interface_protocol;
  *device->next_descriptor_ptr++ = BuildString(device, interface->interface_name);
  *device->num_interfaces_ptr = (unsigned char)device->next_interface_id;
  device->interface_handlers[device->next_interface_id].handler = interface->handler;
  device->interface_handlers[device->next_interface_id].data = data;
  return device->next_interface_id++;
}

int AddEndpointDescriptor(USBDevice *device, const USBEndpointDescriptor *endpoint, void *data)
{
  if (device->next_endpoint_id < 0)
    return -1;
  update_total_length(device, 7);
  *device->next_descriptor_ptr++ = 7; // length
  *device->next_descriptor_ptr++ = endpoint_descriptor_type;
  *device->next_descriptor_ptr++ = device->next_endpoint_id | (endpoint->in_endpoint ? USB_EP_IN : 0);
  unsigned char attributes = endpoint->transfer_type | (endpoint->synchronization_type << 2) | (endpoint->usage_type << 4);
  *device->next_descriptor_ptr++ = attributes;
  *device->next_descriptor_ptr++ = endpoint->max_packet_size & 0xFF;
  *device->next_descriptor_ptr++ = endpoint->max_packet_size >> 8;
  *device->next_descriptor_ptr++ = endpoint->interval;

  USBInitEndpoint(device, device->next_endpoint_id, endpoint->max_packet_size, endpoint->transfer_type);

  int ep = device->next_endpoint_id;
  if (endpoint->endpoint_number_increment)
    device->next_endpoint_id = USBGetNextEndpoint(device, device->next_endpoint_id);

  device->endpoint_packet_lengths[ep] = endpoint->max_packet_size;
  device->endpoint_handlers[ep].handler = endpoint->handler;
  device->endpoint_handlers[ep].handler = data;

  return ep;
}

void USBDeviceActivateSetup(unsigned int device_no, USBDevice *device)
{
  USBEnableEndpoint(device->data, 0, usb_endpoint_direction_inout);
  for (int i = 0; i < device->next_reset_callback; i++)
  {
    usb_callback c = device->Reset_Callbacks[i];
    if (c)
      c(device_no);
    else
      break;
  }
}

void *USBGetDescriptor(USBDevice *device, USBDescriptorType type, unsigned int id, unsigned int *length)
{
  unsigned int l;
  switch (type)
  {
    case device_descriptor_type:
      *length = USB_DEVICE_DESCRIPTOR_SIZE;
      return device->device_descriptor;
    case configuration_descriptor_type:
      *length = device->total_length;
      return device->device_configuration;
    case string_descriptor_type:
      if (!id || id >= device->next_string_id)
        return NULL;
      l = device->string_length[id - 1];
      if (!l)
        return NULL;
      *length = l;
      return device->string_table[id - 1];
    default: return NULL;
  }
}

void USBDeviceStartTransfer(USBDevice *device, int endpoint, const void *buffer, unsigned int length)
{
  unsigned int l = length > USB_FS_MAX_PACKET_SIZE ? USB_FS_MAX_PACKET_SIZE : length;
  memcpy(USBGetEndpointInBuffer(device->data, endpoint), buffer, l);
  USBActivateEndpoint(device->data, endpoint, l);
  length -= l;
  if (length)
  {
    device->endpoints[endpoint].transfer_buffer = buffer + l;
    device->endpoints[endpoint].transfer_length = length;
  }
}

int USBDeviceContinueTransfer(USBDevice *device, int endpoint)
{
  unsigned int l = device->endpoints[endpoint].transfer_length > USB_FS_MAX_PACKET_SIZE ? USB_FS_MAX_PACKET_SIZE : device->endpoints[endpoint].transfer_length;
  memcpy(USBGetEndpointInBuffer(device->data, endpoint), device->endpoints[endpoint].transfer_buffer, l);
  USBActivateEndpoint(device->data, endpoint, l);
  if (!l)
    return 0;
  device->endpoints[endpoint].transfer_length -= l;
  device->endpoints[endpoint].transfer_buffer += l;
  return 1;
}

static void DeviceRequestHandler(USBDevice *device, USBDeviceRequest *request)
{
  unsigned int length;
  switch ((USBRequestType)request->request)
  {
    case usb_request_type_get_descriptor:
      void *descriptor = USBGetDescriptor(device, request->value >> 8, request->value & 0xFF, &length);
      if (descriptor)
        USBDeviceStartTransfer(device, 0, descriptor, request->length > length ? length : request->length);
      else
        USBStallEndpoint(device->data, 0);
      break;
    case usb_request_type_set_address:
      USBSetAddress(device->data, request->value);
      break;
    case usb_request_type_set_configuration:
      break;
    default:
      USBStallEndpoint(device->data, 0);
      break;
  }
}

static void InterfaceRequestHandler(void *data, USBDeviceRequest *request)
{
  //todo
  USBStallEndpoint(data, 0);
}

void USBDeviceDataPacketReceived(USBDevice *device, unsigned int endpoint, unsigned char *buffer, unsigned int length)
{
  //todo
}

void USBDeviceSetupPacketReceived(USBDevice *device, unsigned char *buffer, unsigned int length)
{
  USBDeviceRequest *request = (USBDeviceRequest *)buffer;
  switch (request->request_type & 0x1F)
  {
    case usb_request_recipient_device:
      DeviceRequestHandler(device, request);
      break;
    case usb_request_recipient_interface:
      InterfaceRequestHandler(device->data, request);
      break;
    default:
      USBStallEndpoint(device->data, 0);
  }
}

unsigned int GetNumberOfEndpoints(unsigned int device_id)
{
  USBDevice *device = &usb_devices[device_id];
  return device->next_endpoint_id;
}

int *GetEndpointPacketLengths(unsigned int device_id)
{
  USBDevice *device = &usb_devices[device_id];
  return device->endpoint_packet_lengths;
}

void AddResetCallback(USBDevice *device, usb_callback callback)
{
  device->Reset_Callbacks[device->next_reset_callback++] = callback;
}

void AddSofCallback(USBDevice *device, usb_callback callback)
{
  device->Reset_Callbacks[device->next_sof_callback++] = callback;
}

void AddLPMCallback(USBDevice *device, usb_callback callback)
{
  device->Reset_Callbacks[device->next_lpm_callback++] = callback;
}

void USBDeviceCallLPMCallbacks(int device_no, USBDevice *device)
{
  for (int i = 0; i < device->next_lpm_callback; i++)
  {
    usb_callback c = device->LPM_Callbacks[i];
    if (c)
      c(device_no);
    else
      break;
  }
}

void USBDeviceCallSofCallbacks(int device_no, USBDevice *device)
{
  for (int i = 0; i < device->next_sof_callback; i++)
  {
    usb_callback c = device->SOF_Callbacks[i];
    if (c)
      c(device_no);
    else
      break;
  }
}

/*void USBDeviceInterruptHandler(int device_no)
{
  USBDevice *device = &devices[device_no];
  int endpoint = USBReadInterruptEndpointNumber(device->data);
  if (endpoint >= 0)
  {
    if (USBIsSetupTransaction(device->data))
      SetupTransactionHandler(device);
    else
    {
      unsigned int in_transaction = USBIsTransactionDirectionIN(device->data, endpoint);
      if (in_transaction)
      {
        if (!ContinueTransfer(device, endpoint))
          USBEnableEndpoint(device->data, endpoint);
      }
      else
      {
        OutTransactionHandler(device->data, endpoint);
      }
    }
  }
  USBClearInterruptFlags(device->data);
}*/
