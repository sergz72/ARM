#include <board.h>
#include <string.h>
#include <usb_device.h>

#ifndef USB_DEVICE_MAX_CONFIGURATIONS
#define USB_DEVICE_MAX_CONFIGURATIONS 1
#endif

#ifndef USB_DEVICE_CONFIGURATIONS_SIZE
#define USB_DEVICE_CONFIGURATIONS_SIZE 512
#endif

#ifndef USB_DEVICE_STRINGTABLE_SIZE
#define USB_DEVICE_STRINGTABLE_SIZE 16
#endif

#define USB_EP_IN 0x80

#define USB_DEVICE_DESCRIPTOR_SIZE 18

typedef struct
{
  const void *transfer_buffer;
  unsigned int transfer_length;
} USBEndpoint;

unsigned char next_string_id;
unsigned int next_configuration_id;
unsigned int next_interface_id;
unsigned int next_endpoint_id;
unsigned char *next_string_ptr;
unsigned char *next_descriptor_ptr;
unsigned char *total_length_ptr;
unsigned short total_length[USB_DEVICE_MAX_CONFIGURATIONS];
unsigned char device_descriptor[USB_DEVICE_DESCRIPTOR_SIZE];
unsigned char device_configurations[USB_DEVICE_CONFIGURATIONS_SIZE];
unsigned char *device_configuration[USB_DEVICE_MAX_CONFIGURATIONS];
unsigned char *string_table[USB_DEVICE_STRINGTABLE_SIZE];
unsigned int  string_length[USB_DEVICE_STRINGTABLE_SIZE];
USBEndpoint endpoints[USB_MAX_ENDPOINTS];

static unsigned char BuildString(const char *str)
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

static void BuildDeviceDescriptor(const USBDeviceConfiguration *config)
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
  *ptr = config->num_configurations;
}

static void InitEndpoints(void)
{
  memset(endpoints, 0, sizeof(endpoints));
}

void USBDeviceInit(const USBDeviceConfiguration *config)
{
  next_string_id = 1;
  next_configuration_id = 0;
  next_interface_id = 0;
  next_endpoint_id = 1;
  next_string_ptr = device_configurations + sizeof(device_configurations);
  next_descriptor_ptr = device_configurations;
  memset(total_length, 0, sizeof(total_length));
  memset(string_length, 0, sizeof(string_length));
  BuildDeviceDescriptor(config);
  InitEndpoints();
  USBSetEndpointTransferType(0, usb_endpoint_transfer_type_control);
  USBEnableEndpoint(0);
  config->descriptor_builder();
}


void AddConfigurationDescriptor(const USBConfigurationDescriptor *configuration)
{
  device_configuration[next_configuration_id] = next_descriptor_ptr;
  *next_descriptor_ptr++ = 9; // length
  *next_descriptor_ptr++ = configuration_descriptor_type;
  total_length_ptr = next_descriptor_ptr;
  total_length[next_configuration_id] = 9;
  *next_descriptor_ptr++ = 9;
  *next_descriptor_ptr++ = 0;
  *next_descriptor_ptr++ = configuration->num_interfaces;
  *next_descriptor_ptr++ = next_configuration_id++;
  *next_descriptor_ptr++ = BuildString(configuration->configuration_name);
  unsigned char attributes = 0x80;
  if (configuration->self_powered)
    attributes |= 0x40;
  if (configuration->remote_wakeup)
    attributes |= 0x20;
  *next_descriptor_ptr++ = attributes;
  unsigned char max_power = configuration->max_power >= 510 ? 255 : configuration->max_power / 2;
  *next_descriptor_ptr++ = max_power;
}

static void update_total_length(unsigned short value)
{
  total_length[next_configuration_id] += value;
  total_length_ptr[0] = total_length[next_configuration_id] & 0xFF;
  total_length_ptr[1] = total_length[next_configuration_id] >> 8;
}

void AddInterfaceAssociationDescriptor(const USBInterfaceAssociationDescriptor *interface_association)
{
  update_total_length(8);
  *next_descriptor_ptr++ = 8; // length
  *next_descriptor_ptr++ = interface_association_descriptor_type;
  *next_descriptor_ptr++ = next_interface_id;
  *next_descriptor_ptr++ = interface_association->interface_count;
  *next_descriptor_ptr++ = interface_association->function_class;
  *next_descriptor_ptr++ = interface_association->function_subclass;
  *next_descriptor_ptr++ = interface_association->function_protocol;
  *next_descriptor_ptr++ = BuildString(interface_association->function_name);
}

void AddClassInterfaceDescriptor(unsigned char subtype, unsigned char data1, unsigned char data2)
{
  update_total_length(5);
  *next_descriptor_ptr++ = 5; // length
  *next_descriptor_ptr++ = cs_interface_descriptor_type;
  *next_descriptor_ptr++ = subtype;
  *next_descriptor_ptr++ = data1;
  *next_descriptor_ptr++ = data2;
}

void AddClassInterfaceDescriptor4(unsigned char subtype, unsigned char data1)
{
  update_total_length(4);
  *next_descriptor_ptr++ = 4; // length
  *next_descriptor_ptr++ = cs_interface_descriptor_type;
  *next_descriptor_ptr++ = subtype;
  *next_descriptor_ptr++ = data1;
}

unsigned int AddInterfaceDescriptor(const USBInterfaceDescriptor *interface)
{
  update_total_length(9);
  *next_descriptor_ptr++ = 9; // length
  *next_descriptor_ptr++ = interface_descriptor_type;
  *next_descriptor_ptr++ = next_interface_id;
  *next_descriptor_ptr++ = 0; //alternate setting
  *next_descriptor_ptr++ = interface->number_endpoints;
  *next_descriptor_ptr++ = interface->interface_class;
  *next_descriptor_ptr++ = interface->interface_subclass;
  *next_descriptor_ptr++ = interface->interface_protocol;
  *next_descriptor_ptr++ = BuildString(interface->interface_name);
  return next_interface_id++;
}

unsigned int AddEndpointDescriptor(const USBEndpointDescriptor *endpoint)
{
  update_total_length(7);
  *next_descriptor_ptr++ = 7; // length
  *next_descriptor_ptr++ = endpoint_descriptor_type;
  *next_descriptor_ptr++ = next_endpoint_id | (endpoint->in_endpoint ? USB_EP_IN : 0);
  unsigned char attributes = endpoint->transfer_type | (endpoint->synchronization_type << 2) | (endpoint->usage_type << 4);
  *next_descriptor_ptr++ = attributes;
  *next_descriptor_ptr++ = endpoint->max_packet_size & 0xFF;
  *next_descriptor_ptr++ = endpoint->max_packet_size >> 8;
  *next_descriptor_ptr++ = endpoint->interval;

  USBSetEndpointTransferType(next_endpoint_id, endpoint->transfer_type);
  USBEnableEndpoint(next_endpoint_id);

  return endpoint->endpoint_number_increment ? next_endpoint_id++ : next_endpoint_id;
}

void *USBGetDescriptor(USBDescriptorType type, unsigned int id, unsigned int *length)
{
  unsigned int l;
  switch (type)
  {
    case device_descriptor_type:
      *length = USB_DEVICE_DESCRIPTOR_SIZE;
      return device_descriptor;
    case configuration_descriptor_type:
      l = total_length[id];
      if (id >= next_configuration_id || !l)
        return NULL;
      *length = l;
      return device_configuration[id];
    case string_descriptor_type:
      l = string_length[id - 1];
      if (!id || id >= next_string_id || !l)
        return NULL;
      *length = l;
      return string_table[id - 1];
    default: return NULL;
  }
}

static void StartTransfer(int endpoint, const void *buffer, unsigned int length)
{
  unsigned int l = length > USB_FS_MAX_PACKET_SIZE ? USB_FS_MAX_PACKET_SIZE : length;
  memcpy(USBGetEndpointOutBuffer(endpoint), buffer, l);
  USBActivateEndpoint(endpoint, l);
  length -= l;
  if (length)
  {
    endpoints[endpoint].transfer_buffer = buffer + l;
    endpoints[endpoint].transfer_length = length;
  }
}

static int ContinueTransfer(int endpoint)
{
  unsigned int l = endpoints[endpoint].transfer_length > USB_FS_MAX_PACKET_SIZE ? USB_FS_MAX_PACKET_SIZE : endpoints[endpoint].transfer_length;
  if (!l)
    return 0;
  memcpy(USBGetEndpointOutBuffer(endpoint), endpoints[endpoint].transfer_buffer, l);
  USBActivateEndpoint(endpoint, l);
  endpoints[endpoint].transfer_length -= l;
  endpoints[endpoint].transfer_buffer += l;
  return 1;
}

static void DeviceRequestHandler(int endpoint, USBDeviceRequest *request)
{
  unsigned int length;
  switch ((USBRequestType)request->request)
  {
    case usb_request_type_get_descriptor:
      void *descriptor = USBGetDescriptor(request->value >> 8, request->value & 0xFF, &length);
      if (descriptor)
        StartTransfer(endpoint, descriptor, request->length > length ? length : request->length);
      else
        USBStallEndpoint(endpoint);
      break;
    case usb_request_type_set_configuration:
      break;
    default:
      USBStallEndpoint(endpoint);
    break;
  }
}

void __attribute__((weak)) InterfaceRequestHandler(int endpoint, USBDeviceRequest *request)
{

}

static void InSetupTransactionHandler(int endpoint)
{
  USBDeviceRequest *request = (USBDeviceRequest *)USBGetEndpointInBuffer(endpoint);
  switch (request->request_type & 0x1F)
  {
    case usb_request_recipient_device:
      DeviceRequestHandler(endpoint, request);
      break;
    case usb_request_recipient_interface:
      InterfaceRequestHandler(endpoint, request);
      break;
    default:
      USBStallEndpoint(endpoint);
  }
}

void __attribute__((weak)) InTransactionHandler(int endpoint)
{

}

void USBDeviceInterruptHandler(void)
{
  int endpoint = USBReadInterruptEndpointNumber();
  if (endpoint >= 0)
  {
    int in_transaction = USBIsTransactionDirectionIN(endpoint);
    if (USBIsSetupTransaction(endpoint))
    {
      if (in_transaction)
        InSetupTransactionHandler(endpoint);
    }
    else
    {
      if (in_transaction)
      {
        InTransactionHandler(endpoint);
      }
      else
      {
        if (!ContinueTransfer(endpoint))
          USBEnableEndpoint(endpoint);
      }
    }
  }
  USBClearInterruptFlags();
}