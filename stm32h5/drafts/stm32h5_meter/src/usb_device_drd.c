#include <board.h>
#include <usb_device_drd.h>

static void AssignEndpointBuffers(int endpoint, unsigned int rxaddress, unsigned int txaddress)
{
  USB_DRD_PMABuffDescTypeDef *buf = USB_DRD_PMA_BUFF + endpoint;
  buf->RXBD = rxaddress | (1 << 26) | (1 << 31); // 64 byte buffer
  buf->TXBD = txaddress;
}

static void AssignEndpointsBuffers(void)
{
  unsigned int address = 0;
  for (int i = 0; i < 7; i++)
  {
    AssignEndpointBuffers(i, address, address + 64);
    address += 128;
  }
}
void USBDeviceDrdInit(void)
{
  AssignEndpointsBuffers();
  USB_DRD_FS->CNTR = USB_CNTR_ERRM | USB_CNTR_PMAOVRM | USB_CNTR_CTRM | USB_CNTR_THR512M;
}

int USBReadInterruptEndpointNumber(void)
{
  //todo
  return 0;
}

int USBIsTransactionDirectionIN(int endpoint)
{
  //todo
  return 0;
}

int USBIsSetupTransaction(int endpoint)
{
  //todo
  return 0;
}

void USBEnableEndpoint(unsigned int endpoint)
{
  //todo
}

void USBActivateEndpoint(unsigned int endpoint)
{
  //todo
}

void USBStallEndpoint(unsigned int endpoint)
{
  //todo
}

void *USBGetEndpointInBuffer(int endpoint)
{
  //todo
  return NULL;
}

void *USBGetEndpointOutBuffer(int endpoint)
{
  //todo
  return NULL;
}

void USBSetEndpointTransferType(int endpoint, USBEndpointTransferType transfer_type)
{
  //todo
}

void USBClearInterruptFlags(void)
{
  // clear all interrupt requests
  USB_DRD_FS->ISTR = 0;
}
