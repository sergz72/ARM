#ifndef _USB_DESC_H
#define _USB_DESC_H

#define USB_CONFIG_DESC(num_interfaces) \
    /*Configuration Descriptor*/ \
    0x09,   /* bLength: Configuration Descriptor size */ \
    USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */ \
    USB_DESC_SIZ,                /* wTotalLength:no of returned bytes */ \
    0x00, \
    num_interfaces,   /* bNumInterfaces */ \
    0x01,   /* bConfigurationValue: Configuration value */ \
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */ \
    0xC0,   /* bmAttributes: self powered */ \
    0x32,   /* MaxPower 50 mA */

#endif
