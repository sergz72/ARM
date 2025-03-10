#include <stdio.h>
#include "class/cdc/usb_cdc.h"
#include "usb_test_device.h"

void cdc_rx_callback(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length)
{
  buffer[buffer_length] = 0;
  printf("cdc_rx_callback port %d %s\n", port_id, buffer);
}

int main(void)
{
  USB_CDC_Init(1, 0, 0, 100, cdc_rx_callback);
  USBTestDeviceInit();
  unsigned char *data = SendDescriptorRequest(device_descriptor_type);
  if (data == NULL)
    return 1;
  return 0;
}