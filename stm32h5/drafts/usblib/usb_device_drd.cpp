#include "board.h"
#include <usb_device_drd.h>
#include <cstring>

static void AssignEndpointBuffers(int endpoint, unsigned int rxaddress, unsigned int txaddress)
{
  USB_DRD_PMABuffDescTypeDef *buf = USB_DRD_PMA_BUFF + endpoint;
  buf->RXBD = rxaddress | (1 << 26) | (1 << 31); // 64 byte buffer
  buf->TXBD = txaddress;
}

static void AssignEndpointsBuffers()
{
  unsigned int address = 0;
  for (int i = 0; i < 7; i++)
  {
    AssignEndpointBuffers(i, address, address + 64);
    address += 128;
  }
}

USB_Device_DRD::USB_Device_DRD()
{
  manager = nullptr;
}

void USB_Device_DRD::Init(USB_DeviceManager *m)
{
  manager = m;
  AssignEndpointsBuffers();
  USB_DRD_FS->CNTR = USB_CNTR_ERRM | USB_CNTR_PMAOVRM | USB_CNTR_CTRM | USB_CNTR_THR512M
    | USB_CNTR_SUSPM | USB_CNTR_DCON | USB_CNTR_WKUPM | USB_CNTR_SOFM;
}

void USB_Device_DRD::Reset()
{
  USB_DRD_FS->DADDR = USB_DADDR_EF;
}

void USB_Device_DRD::Connect()
{
  USB_DRD_FS->BCDR = 1 << 15; // enable the embedded pull-up on DP line
}

void USB_Device_DRD::SetEndpointTransferType(unsigned int endpoint, USBEndpointTransferType transfer_type)
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
  value |= endpoint | epkind;
  *reg = value;
}

void USB_Device_DRD::ConfigureEndpoint(unsigned int endpoint_no, USBEndpointConfiguration rx_config,
                                        USBEndpointConfiguration tx_config)
{
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint_no;
  unsigned long value = *reg;
  unsigned int state = value & (USB_CHEP_RX_STRX | USB_CHEP_TX_STTX);
  state ^= (rx_config << 12) | (tx_config << 4);
  value &= ~(USB_CHEP_RX_STRX | USB_CHEP_TX_STTX);
  value |= state;
  *reg = value;
}

void USB_Device_DRD::ConfigureEndpointRX(unsigned int endpoint_no, USBEndpointConfiguration config)
{
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint_no;
  unsigned long value = *reg;
  unsigned int state = value & USB_CHEP_RX_STRX;
  state ^= config << 12;
  value &= ~USB_CHEP_RX_STRX;
  value |= state;
  *reg = value;
}

void USB_Device_DRD::ConfigureEndpointTX(unsigned int endpoint_no, USBEndpointConfiguration config)
{
  volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint_no;
  unsigned long value = *reg;
  unsigned int state = value & USB_CHEP_TX_STTX;
  state ^= config << 4;
  value &= ~USB_CHEP_TX_STTX;
  value |= state;
  *reg = value;
}

void *GetEndpointInBuffer(unsigned int endpoint)
{
  unsigned char *buf = (unsigned char*)USB_DRD_PMAADDR;
  return buf + endpoint * 128;
}

void *GetEndpointOutBuffer(unsigned int endpoint)
{
  unsigned char *buf = (unsigned char*)USB_DRD_PMAADDR;
  return buf + endpoint * 128 + 64;
}

void USB_Device_DRD::SetEndpointData(unsigned endpoint_no, const void *data, unsigned int length)
{
  memcpy(GetEndpointOutBuffer(endpoint_no), data, length);
  USB_DRD_PMABuffDescTypeDef *buff = USB_DRD_PMA_BUFF + endpoint_no;
  unsigned int temp = buff->TXBD & 0xFC00FFFF;
  buff->TXBD = temp | (length << 16);
  ConfigureEndpointTX(endpoint_no, usb_endpoint_configuration_enabled);
}

void USB_Device_DRD::ZeroTransfer(unsigned int endpoint_no)
{
  USB_DRD_PMABuffDescTypeDef *buff = USB_DRD_PMA_BUFF + endpoint_no;
  buff->TXBD &= 0xFC00FFFF;
  ConfigureEndpointTX(endpoint_no, usb_endpoint_configuration_enabled);
}

unsigned int GetEndpointRxLength(unsigned int endpoint)
{
  USB_DRD_PMABuffDescTypeDef *buff = USB_DRD_PMA_BUFF + endpoint;
  return (buff->RXBD & 0x3ff0000) >> 16;
}

void USB_Device_DRD::InterruptHandler()
{
  unsigned int istr = USB_DRD_FS->ISTR;
  if (istr & USB_ISTR_DCON)
  {
    manager->Reset();
    USB_DRD_FS->ISTR &= ~USB_ISTR_DCON;
    return;
  }
  if (istr & USB_ISTR_SUSP)
  {
    USB_DRD_FS->CNTR |= USB_CNTR_SUSPEN;
    istr &= ~USB_ISTR_SUSP;
    manager->Suspend();
  }
  if (istr & USB_ISTR_WKUP)
  {
    manager->Resume();
    istr &= ~USB_ISTR_WKUP;
  }
  if (istr & USB_ISTR_SOF)
  {
    manager->Sof();
    istr &= ~USB_ISTR_SOF;
  }
  if (istr & USB_ISTR_CTR)
  {
    unsigned int endpoint = USB_DRD_FS->ISTR & 0x0F;
    volatile unsigned long *reg = &USB_DRD_FS->CHEP0R + endpoint;
    unsigned long value = *reg;
    if (value & 0x8080) //vttx or vtrx
    {
      unsigned int out = value & 0x8000;
      value &= ~0x8080;
      *reg = value;
      if (out)
      {
        if (value & 0x800) // setup
          manager->SetupPacketReceived(GetEndpointInBuffer(endpoint));
        else
          manager->DataPacketReceived(endpoint, GetEndpointInBuffer(endpoint), GetEndpointRxLength(endpoint));
      }
      else
        manager->ContinueTransfer(endpoint);
    }
  }
  // clear all interrupt requests
  USB_DRD_FS->ISTR = 0;
}

void USB_Device_DRD::SetAddress(unsigned short address)
{
  USB_DRD_FS->DADDR = USB_DADDR_EF | (address & 0x7F);
}

const unsigned int USB_Device_DRD::GetEnabledEndpoints()
{
  return 0xFF;
}
