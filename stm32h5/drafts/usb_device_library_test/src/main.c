#include <stdio.h>
#include "class/cdc/usb_cdc.h"
#include "usb_test_device.h"

unsigned char usb_buffer[1024];

void cdc_rx_callback(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length)
{
  buffer[buffer_length] = 0;
  printf("cdc_rx_callback port %d %s\n", port_id, buffer);
}

int main(void)
{
  if (USB_CDC_Init(1, 0, 0, 100, cdc_rx_callback))
    return 1;
  USBTestDeviceInit();
  if (SendDescriptorRequest(device_descriptor_type, 0, 0, 18, usb_buffer))
    return 1;
  if (SendDescriptorRequest(configuration_descriptor_type, 0, 0, 9, usb_buffer))
    return 1;
  unsigned short total_length = ((unsigned short*)usb_buffer)[1];
  if (SendDescriptorRequest(configuration_descriptor_type, 0, 0, total_length, usb_buffer))
    return 1;
  if (SendDescriptorRequest(string_descriptor_type, 1, 0, 255, usb_buffer))
    return 1;
  if (SendDescriptorRequest(string_descriptor_type, 2, 0, 255, usb_buffer))
    return 1;
  if (SendDescriptorRequest(string_descriptor_type, 3, 0, 255, usb_buffer))
    return 1;
  if (SendDescriptorRequest(string_descriptor_type, 4, 0, 255, usb_buffer))
    return 1;
  if (SendDescriptorRequest(string_descriptor_type, 5, 0, 255, usb_buffer))
    return 1;
  return 0;
}