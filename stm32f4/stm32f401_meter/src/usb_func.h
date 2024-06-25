#ifndef _USB_FUNC_H
#define _USB_FUNC_H

#define USB_FS
#define STM32F3

#define USB_MAX_NUM_INTERFACES               1
#define USB_MAX_NUM_CONFIGURATION            1

#define USB_TRDT_TIME 0x6

#define CDC_RX_DATA_SIZE  1000

#define USB_DEVICECLASS 0x02
#define USB_DESC_SIZ (USB_CDC_CONFIG_DESC_SIZ + USB_CDC_INTERFACE_DESC_SIZ)

#define USB_VID     1155
#define USB_LANGID_STRING     1033
#define USB_MANUFACTURER_STRING     "STMicroelectronics"
#define USB_PID_FS     22336
#define USB_PRODUCT_STRING_FS     "STM32 Virtual ComPort"
#define USB_CONFIGURATION_STRING_FS     "CDC Config"
#define USB_INTERFACE_STRING_FS     "CDC Interface"
#define USB_SERIAL_NUMBER_STRING "00000000001A"

void USBStart(void);
int usb_printf(const char *format, ...);
void puts_(const char *s);

#include <cdc_desc.h>

#endif
