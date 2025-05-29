#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "board.h"

#define USB_FS_MAX_PACKET_SIZE 64

#define USB_INTERRUPT_IN_ENDPOINT 1

#define USB_MAX_ENDPOINTS 16

#ifndef USB_DEVICE_CONFIGURATION_SIZE
#define USB_DEVICE_CONFIGURATION_SIZE 256
#endif

#ifndef USB_DEVICE_STRINGTABLE_SIZE
#define USB_DEVICE_STRINGTABLE_SIZE 16
#endif

#ifndef USB_DEVICE_MAX_CLASSES
#define USB_DEVICE_MAX_CLASSES 4
#endif

#define USB_DEVICE_DESCRIPTOR_SIZE 18

typedef enum
{
  usb_endpoint_transfer_type_control = 0,
  usb_endpoint_transfer_type_isochronous = 1,
  usb_endpoint_transfer_type_bulk = 2,
  usb_endpoint_transfer_type_interrupt = 3
} USBEndpointTransferType;

typedef enum
{
  usb_endpoint_synchronization_type_none = 0,
  usb_endpoint_synchronization_type_asynchronous = 1,
  usb_endpoint_synchronization_type_adaptive = 2,
  usb_endpoint_synchronization_type_synchronous = 3
} USBEndpointSynchronizationType;

typedef enum
{
  usb_endpoint_usage_type_data = 0,
  usb_endpoint_usage_type_feedback = 1,
  usb_endpoint_usage_type_implicit_feedback = 2
} USBEndpointUsageType;

typedef enum
{
  usb_class_unspecified = 0,
  usb_class_audio = 1,
  usb_class_cdc = 2,
  usb_class_hid = 3,
  usb_class_pid = 5,
  usb_class_image = 6,
  usb_class_printer = 7,
  usb_class_mass_storage = 8,
  usb_class_hub = 9,
  usb_class_cdc_data = 10,
  usb_class_smart_card = 11,
  usb_class_security = 13,
  usb_class_video = 14,
  usb_class_healthcare = 15,
  usb_class_diagnostic = 0xDC,
  usb_class_wireless = 0xE0,
  usb_class_misc = 0xEF,
  usb_class_application_specific = 0xFE,
  usb_class_vendor_specific = 0xFF
} USBDeviceClass;

typedef enum
{
  device_descriptor_type = 1,
  configuration_descriptor_type = 2,
  string_descriptor_type = 3,
  interface_descriptor_type = 4,
  endpoint_descriptor_type = 5,
  device_qualifier_descriptor_type = 6,
  other_speed_configuration_descriptor_type = 7,
  interface_association_descriptor_type = 11,
  bos_descriptor_type = 15,
  cs_interface_descriptor_type = 0x24
} USBDescriptorType;

typedef enum
{
  usb_request_type_get_status = 0,
  usb_request_type_clear_feature = 1,
  usb_request_type_set_feature = 3,
  usb_request_type_set_address = 5,
  usb_request_type_get_descriptor = 6,
  usb_request_type_set_descriptor = 7,
  usb_request_type_get_configuration = 8,
  usb_request_type_set_configuration = 9,
  usb_request_type_get_interface = 10,
  usb_request_type_set_interface = 11,
  usb_request_type_sync_frame = 12
} USBRequestType;

typedef enum
{
  usb_request_recipient_device = 0,
  usb_request_recipient_interface = 1,
  usb_request_recipient_endpoint = 2
} USBRequestRecipient;

typedef enum
{
  usb_endpoint_direction_in = 0,
  usb_endpoint_direction_out = 1,
  usb_endpoint_direction_inout = 2,
} USBEndpointDirection;

typedef struct __attribute__((__packed__))
{
  unsigned char request_type;
  unsigned char request;
  unsigned short value;
  unsigned short index;
  unsigned short length;
} USBDeviceRequest;

struct _USBDevice;

typedef int (*usb_descriptor_builder)(unsigned int device_id, struct _USBDevice *device);
typedef void (*usb_callback)(unsigned int device_id);

typedef struct
{
  USBDeviceClass device_class;
  unsigned short vendor_id;
  unsigned short product_id;
  const char *manufacturer;
  const char *product;
  const char *serial_number;
  const char *enabled_endpoints;
  void *data;
  usb_callback Resume_Callback;
  usb_callback Suspend_Callback;
  usb_callback Connect_Callback;
  usb_callback Disconnect_Callback;
} USBDeviceConfiguration;

typedef struct
{
  unsigned char interface_count;
  unsigned char function_class;
  unsigned char function_subclass;
  unsigned char function_protocol;
  char *function_name;
} USBInterfaceAssociationDescriptor;

typedef struct
{
  int endpoint_number_increment;
  int in_endpoint;
  USBEndpointTransferType transfer_type;
  USBEndpointSynchronizationType synchronization_type;
  USBEndpointUsageType usage_type;
  unsigned short max_packet_size;
  unsigned char interval;
} USBEndpointDescriptor;

typedef struct
{
  unsigned char number_endpoints;
  unsigned char interface_class;
  unsigned char interface_subclass;
  unsigned char interface_protocol;
  char *interface_name;
} USBInterfaceDescriptor;

typedef struct
{
  char *configuration_name;
  int self_powered;
  int remote_wakeup;
  unsigned int max_power;
} USBConfigurationDescriptor;

typedef struct
{
  const void *transfer_buffer;
  unsigned int transfer_length;
  int enabled;
} USBEndpoint;

typedef struct _USBDevice
{
  unsigned char next_string_id;
  unsigned int next_interface_id;
  int next_endpoint_id;
  unsigned char *next_string_ptr;
  unsigned char *next_descriptor_ptr;
  unsigned char *total_length_ptr;
  unsigned char *num_interfaces_ptr;
  unsigned short total_length;
  unsigned char device_descriptor[USB_DEVICE_DESCRIPTOR_SIZE];
  unsigned char device_configuration[USB_DEVICE_CONFIGURATION_SIZE];
  unsigned char *string_table[USB_DEVICE_STRINGTABLE_SIZE];
  unsigned int  string_length[USB_DEVICE_STRINGTABLE_SIZE];
  USBEndpoint endpoints[USB_MAX_ENDPOINTS];
  const USBDeviceConfiguration *config;
  void *data;
  usb_callback Reset_Callbacks[USB_DEVICE_MAX_CLASSES];
  usb_callback LPM_Callbacks[USB_DEVICE_MAX_CLASSES];
  usb_callback SOF_Callbacks[USB_DEVICE_MAX_CLASSES];
} USBDevice;

//usb device core functions
unsigned int USBDeviceInit(const USBDeviceConfiguration *config, USBConfigurationDescriptor *configuration_descriptor);
int USBDeviceValidate(unsigned int device_id);
void USBDeviceActivateSetup(unsigned int device_no, USBDevice *device);
void AddInterfaceAssociationDescriptor(USBDevice *device,const USBInterfaceAssociationDescriptor *interface_association);
void AddClassInterfaceDescriptor(USBDevice *device,unsigned char subtype, unsigned char data1, unsigned char data2);
void AddClassInterfaceDescriptor4(USBDevice *device,unsigned char subtype, unsigned char data1);
unsigned int AddInterfaceDescriptor(USBDevice *device, const USBInterfaceDescriptor *interface);
int AddEndpointDescriptor(USBDevice *device, const USBEndpointDescriptor *endpoint);
unsigned int GetNumberOfEndpoints(unsigned int device_id);

//weak functions
void OutTransactionHandler(void *data, int endpoint);
void InterfaceRequestHandler(void *data, USBDeviceRequest *request);

// hal functions
void USBEnableEndpoint(void *data, unsigned int endpoint, USBEndpointDirection direction);
void USBActivateEndpoint(void *data, unsigned int endpoint, unsigned int length);
void USBStallEndpoint(void *data, unsigned int endpoint);
void *USBGetEndpointInBuffer(void *data, int endpoint);
void *USBGetEndpointOutBuffer(void *data, int endpoint);
void USBSetEndpointTransferType(void *data, int endpoint, unsigned int max_packet_size, USBEndpointTransferType transfer_type);
void USBSetAddress(void *data, unsigned short address);
void USBDeviceInterruptHandler(int device_no);

extern USBDevice usb_devices[USB_MAX_DEVICES];

#endif
