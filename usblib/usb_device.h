#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "board.h"

#define USB_FS_MAX_PACKET_SIZE 64

#define USB_MAX_ENDPOINTS 16

#ifndef USB_DEVICE_CONFIGURATION_SIZE
#define USB_DEVICE_CONFIGURATION_SIZE 512
#endif

#ifndef USB_DEVICE_STRINGTABLE_SIZE
#define USB_DEVICE_STRINGTABLE_SIZE 16
#endif

#ifndef USB_DEVICE_MAX_INTERFACES
#define USB_DEVICE_MAX_INTERFACES 8
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

typedef struct __attribute__((__packed__))
{
  unsigned char request_type;
  unsigned char request;
  unsigned short value;
  unsigned short index;
  unsigned short length;
} USBDeviceRequest;

typedef struct
{
  unsigned char interface_count;
  unsigned char function_class;
  unsigned char function_subclass;
  unsigned char function_protocol;
  char *function_name;
} USBInterfaceAssociationDescriptor;

typedef enum
{
  usb_endpoint_direction_none = 0,
  usb_endpoint_direction_in,
  usb_endpoint_direction_out,
  usb_endpoint_direction_inout,
} USBEndpointDirection;

typedef struct
{
  USBEndpointTransferType transfer_type;
  USBEndpointSynchronizationType synchronization_type;
  USBEndpointUsageType usage_type;
  unsigned short max_packet_size;
  unsigned char interval;
  USBEndpointDirection direction;
} USBEndpointDescriptor;

typedef struct
{
  unsigned char number_endpoints;
  unsigned char interface_class;
  unsigned char interface_subclass;
  unsigned char interface_protocol;
  const char *interface_name;
} USBInterfaceDescriptor;

typedef struct
{
  unsigned char num_interfaces;
  const char *configuration_name;
  int self_powered;
  int remote_wakeup;
  unsigned int max_power;
} USBConfigurationDescriptor;

typedef enum
{
  usb_endpoint_configuration_disabled = 0,
  usb_endpoint_configuration_stall = 1,
  usb_endpoint_configuration_nak = 2,
  usb_endpoint_configuration_enabled = 3,
} USBEndpointConfiguration;

class USB_DeviceManager;

class USB_Device
{
  public:
    virtual void Init(USB_DeviceManager *m) = 0;
    virtual void ConfigureEndpoint(unsigned int endpoint_no, USBEndpointConfiguration rx_config,
                                    USBEndpointConfiguration tx_config) = 0;
    virtual void ConfigureEndpointRX(unsigned int endpoint_no, USBEndpointConfiguration config) = 0;
    virtual void ConfigureEndpointTX(unsigned int endpoint_no, USBEndpointConfiguration config) = 0;
    virtual void SetEndpointData(unsigned int endpoint_no, const void *data, unsigned int length) = 0;
    virtual void ZeroTransfer(unsigned int endpoint_no) = 0;
    virtual void InterruptHandler() = 0;
    virtual void SetEndpointTransferType(unsigned int endpoint_no, USBEndpointTransferType transfer_type) = 0;
    virtual void Connect() = 0;
    virtual void Reset() = 0;
    virtual void SetAddress(unsigned short address) = 0;
    virtual const unsigned int GetEnabledEndpoints() = 0;
};

class USB_Class
{
  public:
    virtual void SetupInterface(USBDeviceRequest *request) = 0;
    virtual void InitEndpoint(unsigned int endpoint) = 0;
    virtual void PacketReceived(unsigned int endpoint, void *data, unsigned int length) = 0;
};

typedef struct
{
  unsigned char *transfer_buffer;
  unsigned char *endpoint_buffer;
  unsigned int pending_transfer;
  unsigned int transfer_length;
  unsigned int max_packet_length;
  USBEndpointTransferType transfer_type;
  USBEndpointDirection direction;
  USB_Class *handler;
} USBEndpoint;

typedef struct
{
  USBDeviceClass device_class;
  unsigned short vendor_id;
  unsigned short product_id;
  const char *manufacturer;
  const char *product;
  const char *serial_number;
  unsigned short language_id;
} USBDeviceConfiguration;

class USB_DeviceManager: public USB_Class
{
  private:
    USB_Device *device;
    const USBDeviceConfiguration *config;
    unsigned char next_string_id;
    unsigned int next_interface_id;
    unsigned int next_endpoint_id;
    unsigned char *next_string_ptr;
    unsigned char *next_descriptor_ptr;
    unsigned char *total_length_ptr;
    unsigned char *num_interfaces_ptr;
    unsigned short total_length;
    unsigned char device_descriptor[USB_DEVICE_DESCRIPTOR_SIZE];
    unsigned char device_configuration[USB_DEVICE_CONFIGURATION_SIZE];
    unsigned char *string_table[USB_DEVICE_STRINGTABLE_SIZE];
    unsigned int  string_length[USB_DEVICE_STRINGTABLE_SIZE];
    volatile USBEndpoint endpoints[USB_MAX_ENDPOINTS];
    USB_Class *interface_handlers[USB_DEVICE_MAX_INTERFACES];
    unsigned short set_address;
    unsigned char language_id_descriptor[4];
    unsigned short device_status;

    void InterfaceRequestHandler(USBDeviceRequest *request);
    void ClassInterfaceRequestHandler(USBDeviceRequest *request);
    void DeviceRequestHandler(USBDeviceRequest *request);
    void *GetDescriptor(USBDescriptorType type, unsigned int id, unsigned int *length);
    unsigned char BuildString(const char *str);
    void BuildDeviceDescriptor();
    void InitEndpoints();
    unsigned int GetNextEndpoint(unsigned int endpoint_id);
    void AddConfigurationDescriptor(const USBConfigurationDescriptor *configuration);
    void UpdateTotalLength(unsigned short value);
    void StandardSetupPacketReceived(USBDeviceRequest *request);
    void ClassSetupPacketReceived(USBDeviceRequest *request);
    virtual void VendorSetupPacketReceived(USBDeviceRequest *request);
    void EndpointRequestHandler(USBDeviceRequest *request);
    void AddEndpointDescriptor(const USBEndpointDescriptor *endpoint, unsigned char flag);
public:
    USB_DeviceManager(const USBDeviceConfiguration *conf,
      const USBConfigurationDescriptor *configuration_descriptor, USB_Device *dev);
    int Init();
    void Reset();
    virtual void Suspend();
    virtual void Resume();
    void Sof();
    void SetupPacketReceived(void *data);
    void DataPacketReceived(unsigned int endpoint, void *data, unsigned int length);
    void AddInterfaceAssociationDescriptor(const USBInterfaceAssociationDescriptor *interface_association);
    void AddClassInterfaceDescriptor(unsigned char subtype, unsigned char data1, unsigned char data2);
    void AddClassInterfaceDescriptor4(unsigned char subtype, unsigned char data1);
    unsigned int AddInterfaceDescriptor(USB_Class *handler, const USBInterfaceDescriptor *interface);
    unsigned int AddEndpointDescriptor(USB_Class *handler, const USBEndpointDescriptor *endpoint);
    int ContinueTransfer(unsigned int endpoint);
    unsigned int GetEndpointMaxTransferSize(unsigned int endpoint_no) const;
    USBEndpointDirection GetEndpointDirection(unsigned int endpoint_no) const;
    void InitEndpoint(unsigned int endpoint) override;
    USB_Device *GetDevice();
    void PacketReceived(unsigned int endpoint, void *data, unsigned int length) override;
    void SetupInterface(USBDeviceRequest *request) override;
    int StartTransfer(unsigned int endpoint, const void *buffer, unsigned int length);
};

#endif
