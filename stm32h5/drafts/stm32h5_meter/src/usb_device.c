#include <board.h>
#include <string.h>
#include <usb_device.h>

#ifndef USB_DESCRIPTORS_SIZE
#define USB_DESCRIPTORS_SIZE 512
#endif

#ifndef USB_STRINGTABLE_SIZE
#define USB_STRINGTABLE_SIZE 16
#endif

#define USB_EP_IN 0x80

unsigned char next_string_id;
unsigned int next_configuration_id;
unsigned int next_interface_id;
unsigned int next_endpoint_id;
unsigned char *next_string_ptr;
unsigned char *next_descriptor_ptr;
unsigned char *total_length_ptr;
unsigned short total_length;
unsigned char device_descriptor[USB_DESCRIPTORS_SIZE];
unsigned char *string_table[USB_STRINGTABLE_SIZE];

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
  *--next_string_ptr = length * 2 + 2;
  string_table[next_string_id - 1] = next_string_ptr;

  return next_string_id++;
}

static void AddDeviceConfiguration(const USBDeviceConfiguration *config)
{
  *next_descriptor_ptr++ = 0x12; // length
  *next_descriptor_ptr++ = device_descriptor_type;
  *next_descriptor_ptr++ = 0; // version lo
  *next_descriptor_ptr++ = 2; // version hi
  *next_descriptor_ptr++ = config->device_class;
  *next_descriptor_ptr++ = 2; // subclass
  *next_descriptor_ptr++ = config->device_class == usb_class_misc ? 1 : 0; //protocol
  *next_descriptor_ptr++ = USB_FS_MAX_PACKET_SIZE;
  *next_descriptor_ptr++ = config->vendor_id & 0xFF;
  *next_descriptor_ptr++ = config->vendor_id >> 8;
  *next_descriptor_ptr++ = config->product_id & 0xFF;
  *next_descriptor_ptr++ = config->product_id >> 8;
  *next_descriptor_ptr++ = 0; // release lo
  *next_descriptor_ptr++ = 2; // release hi
  *next_descriptor_ptr++ = BuildString(config->manufacturer);
  *next_descriptor_ptr++ = 0;
  *next_descriptor_ptr++ = BuildString(config->product);
  *next_descriptor_ptr++ = 0;
  *next_descriptor_ptr++ = BuildString(config->serial_number);
  *next_descriptor_ptr++ = 0;
  *next_descriptor_ptr++ = config->num_configurations;
}

void USBDeviceInit(const USBDeviceConfiguration *config)
{
  next_string_id = 1;
  next_configuration_id = 0;
  next_interface_id = 0;
  next_endpoint_id = 1;
  next_string_ptr = device_descriptor + sizeof(device_descriptor);
  next_descriptor_ptr = device_descriptor;
  AddDeviceConfiguration(config);
  USBActivateEndpoint(0);
  config->descriptor_builder();
}


void AddConfigurationDescriptor(const USBConfigurationDescriptor *configuration)
{
  *next_descriptor_ptr++ = 9; // length
  *next_descriptor_ptr++ = configuration_descriptor_type;
  total_length_ptr = next_descriptor_ptr;
  total_length = 9;
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
  total_length += value;
  total_length_ptr[0] = total_length & 0xFF;
  total_length_ptr[1] = total_length >> 8;
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

  USBEnableEndpoint(next_endpoint_id);

  return endpoint->endpoint_number_increment ? next_endpoint_id++ : next_endpoint_id;
}

void *USBGetDescriptor(USBDescriptorType type, unsigned int id, unsigned int *length)
{
  //todo
  return NULL;
}

static void InSetupTransactionHandler(int endpoint)
{
  unsigned int length;
  USBDeviceRequest *request = (USBDeviceRequest *)USBGetEndpointInBuffer(endpoint);
  switch ((USBRequestType)request->request)
  {
    case usb_request_type_get_descriptor:
      void *descriptor = USBGetDescriptor(request->value & 0xFF, request->value >> 8, &length);
      break;
    default:
      USBStallEndpoint(endpoint);
      break;
  }
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
  }
  USBClearInterruptFlags();
}