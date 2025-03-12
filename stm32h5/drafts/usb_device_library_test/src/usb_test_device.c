#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usb_test_device.h>
#include <usb_device.h>

int interrupt_endpoint_number;
int transaction_direction_in;
int is_setup_transaction;
unsigned int out_packet_size;
void *in_transaction_data[USB_MAX_ENDPOINTS];
void *out_transaction_data[USB_MAX_ENDPOINTS];

void USBTestDeviceInit(void)
{
  puts("USBTestDeviceInit");
  interrupt_endpoint_number = -1;
  transaction_direction_in = 0;
  is_setup_transaction = 0;
  out_packet_size = 0;
  for (int i = 0; i < USB_MAX_ENDPOINTS; i++)
  {
    in_transaction_data[i] = malloc(USB_FS_MAX_PACKET_SIZE);
    out_transaction_data[i] = malloc(USB_FS_MAX_PACKET_SIZE);
  }
}

int USBReadInterruptEndpointNumber(void)
{
  return interrupt_endpoint_number;
}

int USBIsTransactionDirectionIN(int endpoint)
{
  return transaction_direction_in;
}

int USBIsSetupTransaction(int endpoint)
{
  return is_setup_transaction;
}

void USBEnableEndpoint(unsigned int endpoint)
{
  printf("USBEnableEndpoint %d\n", endpoint);
}

void USBActivateEndpoint(unsigned int endpoint, unsigned int length)
{
  out_packet_size = length;
  printf("USBActivateEndpoint %d %d\n", endpoint, length);
}

void USBStallEndpoint(unsigned int endpoint)
{
  printf("USBStallEndpoint %d\n", endpoint);
}

void *USBGetEndpointInBuffer(int endpoint)
{
  return in_transaction_data[endpoint];
}

void *USBGetEndpointOutBuffer(int endpoint)
{
  return out_transaction_data[endpoint];
}

void USBSetEndpointTransferType(int endpoint, USBEndpointTransferType transfer_type)
{
  printf("USBSetEndpointTransferType %d %d\n", endpoint, transfer_type);
}

void USBClearInterruptFlags(void)
{
  puts("USBClearInterruptFlags");
}

int SendDescriptorRequest(USBDescriptorType type, unsigned short id, unsigned short index, unsigned short length, unsigned char *buffer)
{
  interrupt_endpoint_number = 0;
  is_setup_transaction = 1;
  transaction_direction_in = 1;
  USBDeviceRequest *request = (USBDeviceRequest *)in_transaction_data[0];
  request->request_type = 0x80;
  request->request = usb_request_type_get_descriptor;
  request->value = (type << 8) | id;
  request->index = index;
  request->length = length;
  USBDeviceInterruptHandler();
  is_setup_transaction = 0;
  transaction_direction_in = 0;
  while (length)
  {
    memcpy(buffer, out_transaction_data[0], out_packet_size);
    if (out_packet_size < USB_FS_MAX_PACKET_SIZE)
      break;
    if (out_packet_size <= length)
      length -= out_packet_size;
    else
      return 1;
    buffer += out_packet_size;
    if (length)
      USBDeviceInterruptHandler();
  }
  return 0;
}

int USBIsErrorInterrupt(void)
{
  return 0;
}
