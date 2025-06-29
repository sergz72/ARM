#include "board.h"
#include <usb_device_ch32x035.h>
#include <cstring>
#include <ch32x035_usb.h>
#include <stdlib.h>

#define USB_IOEN     0x80
#define USB_PHY_V33  0x40

#define UDP_PUE_MASK 0x0C
#define UDP_PUE_1K5  0x0C

#define UDM_PUE_MASK 0x03

unsigned int USB_Device_CH32X035::CreateEndpointBuffer(unsigned int endpoint_no)
{
  unsigned char *buffer;

  switch (manager->GetEndpointDirection(endpoint_no))
  {
    case usb_endpoint_direction_in:
      buffer = (unsigned char *)malloc(USB_FS_MAX_PACKET_SIZE);
      endpoint_buffers_rx[endpoint_no] = buffer;
      endpoint_buffers_tx[endpoint_no] = nullptr;
      break;
    case usb_endpoint_direction_out:
      buffer = (unsigned char *)malloc(USB_FS_MAX_PACKET_SIZE);
      endpoint_buffers_rx[endpoint_no] = nullptr;
      endpoint_buffers_tx[endpoint_no] = buffer;
      break;
    case usb_endpoint_direction_inout:
      buffer = (unsigned char *)malloc(USB_FS_MAX_PACKET_SIZE*2);
      endpoint_buffers_rx[endpoint_no] = buffer;
      endpoint_buffers_tx[endpoint_no] = buffer + USB_FS_MAX_PACKET_SIZE;
      break;
    default:
      buffer = endpoint_buffers_tx[endpoint_no] = endpoint_buffers_rx[endpoint_no] = nullptr;
      break;
  }

  return (unsigned int)buffer;
}

void USB_Device_CH32X035::AssignEndpointsBuffers()
{
  endpoint_buffers_rx[0] = (unsigned char*)malloc(USB_FS_MAX_PACKET_SIZE);
  endpoint_buffers_tx[0] = endpoint_buffers_rx[0];
  USBFSD->UEP0_DMA = (unsigned int)endpoint_buffers_rx[0];
  USBFSD->UEP1_DMA = CreateEndpointBuffer(1);
  USBFSD->UEP2_DMA = CreateEndpointBuffer(2);
  USBFSD->UEP3_DMA = CreateEndpointBuffer(3);
  USBFSD->UEP5_DMA = CreateEndpointBuffer(5);
  USBFSD->UEP6_DMA = CreateEndpointBuffer(6);
  USBFSD->UEP7_DMA = CreateEndpointBuffer(7);
}

static unsigned char BuildEndpointMode(USBEndpointDirection direction)
{
  switch (direction)
  {
    case usb_endpoint_direction_in:
      return 2;
    case usb_endpoint_direction_out:
      return 1;
    case usb_endpoint_direction_inout:
      return 3;
    default:
      return 0;
  }
}

void USB_Device_CH32X035::AssignEndpointsModes()
{
  USBFSD->UEP4_1_MOD = BuildEndpointMode(manager->GetEndpointDirection(1)) << 6;
  USBFSD->UEP2_3_MOD = (BuildEndpointMode(manager->GetEndpointDirection(2)) << 2) |
                       (BuildEndpointMode(manager->GetEndpointDirection(3)) << 6);
  USBFSD->UEP567_MOD = BuildEndpointMode(manager->GetEndpointDirection(5)) |
                       (BuildEndpointMode(manager->GetEndpointDirection(6)) << 2) |
                       (BuildEndpointMode(manager->GetEndpointDirection(7)) << 4);
}

USB_Device_CH32X035::USB_Device_CH32X035()
{
  manager = nullptr;
}

void USB_Device_CH32X035::Init(USB_DeviceManager *m)
{
  manager = m;

  AssignEndpointsBuffers();
  AssignEndpointsModes();

  // VDD = 3.3v, D+ line pullup = 1.5k
  AFIO->CTLR = (AFIO->CTLR & ~(UDP_PUE_MASK | UDM_PUE_MASK )) | USB_PHY_V33 | UDP_PUE_1K5 | USB_IOEN;

  USBFSD->DEV_ADDR = 0;
  USBFSD->BASE_CTRL = USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;
  USBFSD->INT_FG = 0xFF;
  USBFSD->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;
  USBFSD->INT_EN = USBFS_UIE_SUSPEND | USBFS_UIE_BUS_RST | USBFS_UIE_TRANSFER;

  NVIC_EnableIRQ(USBFS_IRQn);
}

void USB_Device_CH32X035::Reset()
{
  USBFSD->DEV_ADDR = 0;
}

void USB_Device_CH32X035::Connect()
{
  USBFSD->BASE_CTRL |= USBFS_UC_DEV_PU_EN;
}

void USB_Device_CH32X035::SetEndpointTransferType(unsigned int endpoint, USBEndpointTransferType transfer_type)
{
}

static unsigned short BuildResponseControl(USBEndpointConfiguration config)
{
  switch (config)
  {
    case usb_endpoint_configuration_enabled:
      return 0;
    case usb_endpoint_configuration_nak:
      return 2;
    case usb_endpoint_configuration_stall:
      return 3;
    default:
      return 1; // nyet
  }
}
void USB_Device_CH32X035::ConfigureEndpoint(unsigned int endpoint_no, USBEndpointConfiguration rx_config,
                                            USBEndpointConfiguration tx_config)
{
  unsigned short temp;

  switch (endpoint_no)
  {
    case 0:
      temp = USBFSD->UEP0_CTRL_H & 0xF0;
      temp |= (BuildResponseControl(rx_config) << 2) | BuildResponseControl(tx_config);
      USBFSD->UEP0_CTRL_H = temp;
      break;
    case 1:
      temp = USBFSD->UEP1_CTRL_H & 0xF0;
      temp |= (BuildResponseControl(rx_config) << 2) | BuildResponseControl(tx_config);
      USBFSD->UEP1_CTRL_H = temp;
      break;
    case 2:
      temp = USBFSD->UEP2_CTRL_H & 0xF0;
      temp |= (BuildResponseControl(rx_config) << 2) | BuildResponseControl(tx_config);
      USBFSD->UEP2_CTRL_H = temp;
      break;
    case 3:
      temp = USBFSD->UEP3_CTRL_H & 0xF0;
      temp |= (BuildResponseControl(rx_config) << 2) | BuildResponseControl(tx_config);
      USBFSD->UEP3_CTRL_H = temp;
      break;
    case 5:
      temp = USBFSD->UEP5_CTRL_H & 0xF0;
      temp |= (BuildResponseControl(rx_config) << 2) | BuildResponseControl(tx_config);
      USBFSD->UEP5_CTRL_H = temp;
      break;
    case 6:
      temp = USBFSD->UEP6_CTRL_H & 0xF0;
      temp |= (BuildResponseControl(rx_config) << 2) | BuildResponseControl(tx_config);
      USBFSD->UEP6_CTRL_H = temp;
      break;
    case 7:
      temp = USBFSD->UEP7_CTRL_H & 0xF0;
      temp |= (BuildResponseControl(rx_config) << 2) | BuildResponseControl(tx_config);
      USBFSD->UEP7_CTRL_H = temp;
      break;
  }
}

void USB_Device_CH32X035::ConfigureEndpointRX(unsigned int endpoint_no, USBEndpointConfiguration config)
{
  unsigned short temp;

  switch (endpoint_no)
  {
    case 0:
      temp = USBFSD->UEP0_CTRL_H & 0xF3;
      temp |= BuildResponseControl(config) << 2;
      USBFSD->UEP0_CTRL_H = temp;
      break;
    case 1:
      temp = USBFSD->UEP1_CTRL_H & 0xF3;
      temp |= BuildResponseControl(config) << 2;
      USBFSD->UEP1_CTRL_H = temp;
      break;
    case 2:
      temp = USBFSD->UEP2_CTRL_H & 0xF3;
      temp |= BuildResponseControl(config) << 2;
      USBFSD->UEP2_CTRL_H = temp;
      break;
    case 3:
      temp = USBFSD->UEP3_CTRL_H & 0xF3;
      temp |= BuildResponseControl(config) << 2;
      USBFSD->UEP3_CTRL_H = temp;
      break;
    case 5:
      temp = USBFSD->UEP5_CTRL_H & 0xF3;
      temp |= BuildResponseControl(config) << 2;
      USBFSD->UEP5_CTRL_H = temp;
      break;
    case 6:
      temp = USBFSD->UEP6_CTRL_H & 0xF3;
      temp |= BuildResponseControl(config) << 2;
      USBFSD->UEP6_CTRL_H = temp;
      break;
    case 7:
      temp = USBFSD->UEP7_CTRL_H & 0xF3;
      temp |= BuildResponseControl(config) << 2;
      USBFSD->UEP7_CTRL_H = temp;
      break;
  }
}

void USB_Device_CH32X035::ConfigureEndpointTX(unsigned int endpoint_no, USBEndpointConfiguration config)
{
  unsigned short temp;

  switch (endpoint_no)
  {
    case 0:
      temp = USBFSD->UEP0_CTRL_H & 0xFC;
      temp |= BuildResponseControl(config);
      USBFSD->UEP0_CTRL_H = temp;
      break;
    case 1:
      temp = USBFSD->UEP1_CTRL_H & 0xFC;
      temp |= BuildResponseControl(config);
      USBFSD->UEP1_CTRL_H = temp;
      break;
    case 2:
      temp = USBFSD->UEP2_CTRL_H & 0xFC;
      temp |= BuildResponseControl(config);
      USBFSD->UEP2_CTRL_H = temp;
      break;
    case 3:
      temp = USBFSD->UEP3_CTRL_H & 0xFC;
      temp |= BuildResponseControl(config);
      USBFSD->UEP3_CTRL_H = temp;
      break;
    case 5:
      temp = USBFSD->UEP5_CTRL_H & 0xFC;
      temp |= BuildResponseControl(config);
      USBFSD->UEP5_CTRL_H = temp;
      break;
    case 6:
      temp = USBFSD->UEP6_CTRL_H & 0xFC;
      temp |= BuildResponseControl(config);
      USBFSD->UEP6_CTRL_H = temp;
      break;
    case 7:
      temp = USBFSD->UEP7_CTRL_H & 0xFC;
      temp |= BuildResponseControl(config);
      USBFSD->UEP7_CTRL_H = temp;
      break;
  }
}

void USB_Device_CH32X035::SetEndpointData(unsigned endpoint_no, const void *data, unsigned int length)
{
  memcpy(endpoint_buffers_tx[endpoint_no], data, length);
  switch (endpoint_no)
  {
    case 0: USBFSD->UEP0_TX_LEN = length; break;
    case 1: USBFSD->UEP1_TX_LEN = length; break;
    case 2: USBFSD->UEP2_TX_LEN = length; break;
    case 3: USBFSD->UEP3_TX_LEN = length; break;
    case 5: USBFSD->UEP5_TX_LEN = length; break;
    case 6: USBFSD->UEP6_TX_LEN = length; break;
    case 7: USBFSD->UEP7_TX_LEN = length; break;
    default: break;
  }
  ConfigureEndpointTX(endpoint_no, usb_endpoint_configuration_enabled);
}

void USB_Device_CH32X035::ZeroTransfer(unsigned int endpoint_no)
{
  switch (endpoint_no)
  {
    case 0: USBFSD->UEP0_TX_LEN = 0; break;
    case 1: USBFSD->UEP1_TX_LEN = 0; break;
    case 2: USBFSD->UEP2_TX_LEN = 0; break;
    case 3: USBFSD->UEP3_TX_LEN = 0; break;
    case 5: USBFSD->UEP5_TX_LEN = 0; break;
    case 6: USBFSD->UEP6_TX_LEN = 0; break;
    case 7: USBFSD->UEP7_TX_LEN = 0; break;
    default: break;
  }
  ConfigureEndpointTX(endpoint_no, usb_endpoint_configuration_enabled);
}

void USB_Device_CH32X035::SetAddress(unsigned short address)
{
  USBFSD->DEV_ADDR = (USBFSD->DEV_ADDR & USBFS_UDA_GP_BIT) | (uint8_t)address;
}

const unsigned int USB_Device_CH32X035::GetEnabledEndpoints()
{
  return 0xEF; // EP4 is disabled
}

void ToggleRX(unsigned int endpoint)
{
  unsigned short temp;

  switch (endpoint)
  {
    case 0:
      temp = USBFSD->UEP0_CTRL_H & ~USBFS_UEP_R_RES_MASK;
      temp ^= USBFS_UEP_R_TOG;
      temp |= USBFS_UEP_R_RES_NAK;
      USBFSD->UEP0_CTRL_H = temp;
      break;
    case 1:
      temp = USBFSD->UEP1_CTRL_H & ~USBFS_UEP_R_RES_MASK;
      temp ^= USBFS_UEP_R_TOG;
      temp |= USBFS_UEP_R_RES_NAK;
      USBFSD->UEP1_CTRL_H = temp;
      break;
    case 2:
      temp = USBFSD->UEP2_CTRL_H & ~USBFS_UEP_R_RES_MASK;
      temp ^= USBFS_UEP_R_TOG;
      temp |= USBFS_UEP_R_RES_NAK;
      USBFSD->UEP2_CTRL_H = temp;
      break;
    case 3:
      temp = USBFSD->UEP3_CTRL_H & ~USBFS_UEP_R_RES_MASK;
      temp ^= USBFS_UEP_R_TOG;
      temp |= USBFS_UEP_R_RES_NAK;
      USBFSD->UEP3_CTRL_H = temp;
      break;
    case 5:
      temp = USBFSD->UEP5_CTRL_H & ~USBFS_UEP_R_RES_MASK;
      temp ^= USBFS_UEP_R_TOG;
      temp |= USBFS_UEP_R_RES_NAK;
      USBFSD->UEP5_CTRL_H = temp;
      break;
    case 6:
      temp = USBFSD->UEP6_CTRL_H & ~USBFS_UEP_R_RES_MASK;
      temp ^= USBFS_UEP_R_TOG;
      temp |= USBFS_UEP_R_RES_NAK;
      USBFSD->UEP6_CTRL_H = temp;
      break;
    case 7:
      temp = USBFSD->UEP7_CTRL_H & ~USBFS_UEP_R_RES_MASK;
      temp ^= USBFS_UEP_R_TOG;
      temp |= USBFS_UEP_R_RES_NAK;
      USBFSD->UEP7_CTRL_H = temp;
      break;
    default: break;
  }
}

void ToggleTX(unsigned int endpoint)
{
  switch (endpoint)
  {
    case 0: USBFSD->UEP0_CTRL_H ^= USBFS_UEP_T_TOG; break;
    case 1: USBFSD->UEP1_CTRL_H ^= USBFS_UEP_T_TOG; break;
    case 2: USBFSD->UEP2_CTRL_H ^= USBFS_UEP_T_TOG; break;
    case 3: USBFSD->UEP3_CTRL_H ^= USBFS_UEP_T_TOG; break;
    case 5: USBFSD->UEP5_CTRL_H ^= USBFS_UEP_T_TOG; break;
    case 6: USBFSD->UEP6_CTRL_H ^= USBFS_UEP_T_TOG; break;
    case 7: USBFSD->UEP7_CTRL_H ^= USBFS_UEP_T_TOG; break;
    default: break;
  }
}

void USB_Device_CH32X035::InterruptHandler()
{
  uint8_t  intflag, intst;

  intflag = USBFSD->INT_FG;
  intst   = USBFSD->INT_ST;

  if (intflag & USBFS_UIF_TRANSFER)
  {
    unsigned int endpoint = intst & 0x0F;
    switch (intst & USBFS_UIS_TOKEN_MASK)
    {
      case USBFS_UIS_TOKEN_IN:
      {
        ToggleTX(endpoint);
        manager->ContinueTransfer(endpoint);
        break;
      }
      case USBFS_UIS_TOKEN_OUT:
        if (intst & USBFS_UIS_TOG_OK)
        {
          ToggleRX(endpoint);
          unsigned int l = USBFSD->RX_LEN;
          manager->DataPacketReceived(endpoint, endpoint_buffers_rx[endpoint], l);
        }
        break;
      case USBFS_UIS_TOKEN_SETUP:
        USBFSD->UEP0_CTRL_H = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_NAK|USBFS_UEP_R_TOG|USBFS_UEP_R_RES_NAK;
        manager->SetupPacketReceived(endpoint_buffers_rx[0]);
        break;
      default: break;
    }
    USBFSD->INT_FG = USBFS_UIF_TRANSFER;
    return;
  }
  if( intflag & USBFS_UIF_BUS_RST )
  {
    manager->Reset();
    USBFSD->INT_FG = USBFS_UIF_BUS_RST;
    return;
  }
  /* other interrupts */
  USBFSD->INT_FG = intflag;
}
