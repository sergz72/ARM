#ifndef _CDC_DESC_H
#define _CDC_DESC_H

#define CDC_IAD(interface_control_no) \
/*Interface association Descriptor */ \
    0x08, \
    0x0B, \
    interface_control_no, \
    0x02, \
    0x02, \
    0x02, \
    0x01, \
    0x06,

#define CDC_INTERFACE_DESC(interface_control_no, interface_data_no, control_endpoint_no, data_endpoint_no) \
    /*Interface Descriptor */ \
    0x09,   /* bLength: Interface Descriptor size */ \
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */ \
    /* Interface descriptor type */ \
    interface_control_no,   /* bInterfaceNumber: Number of Interface */ \
    0x00,   /* bAlternateSetting: Alternate setting */ \
    0x01,   /* bNumEndpoints: One endpoints used */ \
    0x02,   /* bInterfaceClass: Communication Interface Class */ \
    0x02,   /* bInterfaceSubClass: Abstract Control Model */ \
    0x01,   /* bInterfaceProtocol: Common AT commands */ \
    0x00,   /* iInterface: */ \
    /*Header Functional Descriptor*/ \
    0x05,   /* bLength: Endpoint Descriptor size */ \
    0x24,   /* bDescriptorType: CS_INTERFACE */ \
    0x00,   /* bDescriptorSubtype: Header Func Desc */ \
    0x10,   /* bcdCDC: spec release number */ \
    0x01, \
    /*Call Management Functional Descriptor*/ \
    0x05,   /* bFunctionLength */ \
    0x24,   /* bDescriptorType: CS_INTERFACE */ \
    0x01,   /* bDescriptorSubtype: Call Management Func Desc */ \
    0x00,   /* bmCapabilities: D0+D1 */ \
    interface_data_no,   /* bDataInterface: 1 */ \
    /*ACM Functional Descriptor*/ \
    0x04,   /* bFunctionLength */ \
    0x24,   /* bDescriptorType: CS_INTERFACE */ \
    0x02,   /* bDescriptorSubtype: Abstract Control Management desc */ \
    0x02,   /* bmCapabilities */ \
    /*Union Functional Descriptor*/ \
    0x05,   /* bFunctionLength */ \
    0x24,   /* bDescriptorType: CS_INTERFACE */ \
    0x06,   /* bDescriptorSubtype: Union func desc */ \
    0x00,   /* bMasterInterface: Communication class interface */ \
    interface_data_no,   /* bSlaveInterface0: Data Class Interface */ \
    /*Endpoint 2 Descriptor*/ \
    0x07,                           /* bLength: Endpoint Descriptor size */ \
    USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */ \
    control_endpoint_no | USB_EP_IN,                     /* bEndpointAddress */ \
    0x03,                           /* bmAttributes: Interrupt */ \
    LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */ \
    HIBYTE(CDC_CMD_PACKET_SIZE), \
    0x10,                           /* bInterval: */ \
    /*---------------------------------------------------------------------------*/ \
    /*Data class interface descriptor*/ \
    0x09,   /* bLength: Endpoint Descriptor size */ \
    USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */ \
    interface_data_no,   /* bInterfaceNumber: Number of Interface */ \
    0x00,   /* bAlternateSetting: Alternate setting */ \
    0x02,   /* bNumEndpoints: Two endpoints used */ \
    0x0A,   /* bInterfaceClass: CDC */ \
    0x00,   /* bInterfaceSubClass: */ \
    0x00,   /* bInterfaceProtocol: */ \
    0x00,   /* iInterface: */ \
    /*Endpoint OUT Descriptor*/ \
    0x07,   /* bLength: Endpoint Descriptor size */ \
    USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */ \
    data_endpoint_no,                        /* bEndpointAddress */ \
    0x02,                              /* bmAttributes: Bulk */ \
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */ \
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), \
    0x00,                              /* bInterval: ignore for Bulk transfer */ \
    /*Endpoint IN Descriptor*/ \
    0x07,   /* bLength: Endpoint Descriptor size */ \
    USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */ \
    data_endpoint_no | USB_EP_IN,                         /* bEndpointAddress */ \
    0x02,                              /* bmAttributes: Bulk */ \
    LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */ \
    HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), \
    0x00                               /* bInterval: ignore for Bulk transfer */

#define USB_CDC_CONFIG_DESC_SIZ                     9
#define USB_CDC_INTERFACE_DESC_SIZ                     58

#endif
