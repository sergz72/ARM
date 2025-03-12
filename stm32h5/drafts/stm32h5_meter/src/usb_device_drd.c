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
  USB_DRD_FS->BCDR = 1 << 15; // enable the embedded pull-up on DP line
}

int USBReadInterruptEndpointNumber(void)
{
  int endpoint = USB_DRD_FS->ISTR & 0x0F;
  if (endpoint > 7)
    return -1;
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint;
  unsigned long value = *reg;
  if (value & 0x8080) //vttx or vtrx
  {
    value &= ~0x8080;
    *reg = value;
  }
  else
    return -1;
  return endpoint;
}

int USBIsTransactionDirectionIN(int endpoint)
{
  return USB_DRD_FS->ISTR & 0x10 == 0;
}

int USBIsSetupTransaction(int endpoint)
{
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint;
  return *reg & (1 << 11);
}

void USBEnableEndpoint(unsigned int endpoint)
{
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint;
  unsigned long value = *reg;
  value &= ~(1 << 4);
  value |= 1 << 5;
  *reg = value;
}

void USBActivateEndpoint(unsigned int endpoint, unsigned int length)
{
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint;
  *reg |= 3 << 4;
}

void USBStallEndpoint(unsigned int endpoint)
{
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint;
  unsigned long value = *reg;
  value &= ~(1 << 5);
  value |= 1 << 4;
  *reg = value;
}

void *USBGetEndpointInBuffer(int endpoint)
{
  void *buf = (void*)USB_DRD_PMAADDR;
  return buf + endpoint * 128;
}

void *USBGetEndpointOutBuffer(int endpoint)
{
  void *buf = (void*)USB_DRD_PMAADDR;
  return buf + endpoint * 128 + 64;
}

void USBSetEndpointTransferType(int endpoint, USBEndpointTransferType transfer_type)
{
  endpoint &= 0x0F;
  unsigned long epkind;
  switch (transfer_type)
  {
    case usb_endpoint_transfer_type_control: epkind = 1 << 9; break;
    case usb_endpoint_transfer_type_isochronous: epkind = 2 << 9; break;
    case usb_endpoint_transfer_type_interrupt: epkind = 3 << 9; break;
    default: epkind = 0; break;
  }
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint;
  unsigned long value = *reg;
  value |= endpoint | epkind | (3<<12); //statrx = valid
  *reg = value;
}

void USBClearInterruptFlags(void)
{
  // clear all interrupt requests
  USB_DRD_FS->ISTR = 0;
}
