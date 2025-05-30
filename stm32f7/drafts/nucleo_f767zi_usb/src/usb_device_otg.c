#include <board.h>
#include <usb_device_otg.h>

#define STS_GOUT_NAK                           1U
#define STS_DATA_UPDT                          2U
#define STS_XFER_COMP                          3U
#define STS_SETUP_COMP                         4U
#define STS_SETUP_UPDT                         6U

#define DCFG_FRAME_INTERVAL_80                 0U
#define DCFG_FRAME_INTERVAL_85                 1U
#define DCFG_FRAME_INTERVAL_90                 2U
#define DCFG_FRAME_INTERVAL_95                 3U

#define USB_OTG_SPEED_HIGH                     0U
#define USB_OTG_SPEED_HIGH_IN_FULL             1U
#define USB_OTG_SPEED_LOW                      2U
#define USB_OTG_SPEED_FULL                     3U

#define USBx_PCGCCTL(instance)    *(__IO unsigned int*)((unsigned int)instance + USB_OTG_PCGCCTL_BASE)

#define USBx_DEVICE(instance)     ((USB_OTG_DeviceTypeDef *)((unsigned int)instance + USB_OTG_DEVICE_BASE))
#define USBx_INEP(instance, i)    ((USB_OTG_INEndpointTypeDef *)((unsigned int)instance + USB_OTG_IN_ENDPOINT_BASE + (i)*USB_OTG_EP_REG_SIZE))
#define USBx_OUTEP(instance, i)   ((USB_OTG_OUTEndpointTypeDef *)((unsigned int)instance + USB_OTG_OUT_ENDPOINT_BASE + (i)*USB_OTG_EP_REG_SIZE))

#define CLEAR_IN_EP_INTR(instance, __EPNUM__, __INTERRUPT__)          (USBx_INEP(instance, __EPNUM__)->DIEPINT = (__INTERRUPT__))
#define CLEAR_OUT_EP_INTR(instance, __EPNUM__, __INTERRUPT__)         (USBx_OUTEP(instance, __EPNUM__)->DOEPINT = (__INTERRUPT__))

#define USB_MASK_INTERRUPT(instance, __INTERRUPT__)     (instance->GINTMSK &= ~(__INTERRUPT__))
#define USB_UNMASK_INTERRUPT(instance, __INTERRUPT__)   (instance->GINTMSK |= (__INTERRUPT__))

#define USBx_DFIFO(instance, i)   *(__IO unsigned int*)((unsigned int)instance + USB_OTG_FIFO_BASE + (i) * USB_OTG_FIFO_SIZE)

const char otg_endpoints[USB_MAX_ENDPOINTS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

static void USB_CoreReset(USB_OTG_GlobalTypeDef *instance)
{
  unsigned int count = 0;

  /* Wait for AHB master IDLE state. */
  do
  {
    if (++count > 200000)
    {
      while (1);//timeout
    }
  }
  while ((instance->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0);

  /* Core Soft Reset */
  count = 0;
  instance->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;

  do
  {
    if (++count > 200000)
    {
      while (1);//timeout
    }
  }
  while ((instance->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
}

static void USB_CoreInit(const USB_OTG_CfgTypeDef *cfg)
{
  if (cfg->instance == USB_OTG_FS)
  {
    /* Select FS Embedded PHY */
    cfg->instance->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;

    /* Reset after a PHY select and set Host mode */
    USB_CoreReset(cfg->instance);

    /* Enable USB FS PHY */
    cfg->instance->GCCFG = USB_OTG_GCCFG_PWRDWN;
  }
  else
  {
    cfg->instance->GCCFG &= ~USB_OTG_GCCFG_PWRDWN;

    /* Init The ULPI Interface */
    cfg->instance->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);

    /* Select vbus source */
    cfg->instance->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
    if(cfg->use_external_vbus)
    {
      cfg->instance->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
    }
    /* Reset after a PHY select  */
    USB_CoreReset(cfg->instance);
  }

  if (cfg->dma_enable)
  {
    cfg->instance->GAHBCFG |= USB_OTG_GAHBCFG_HBSTLEN_1 | USB_OTG_GAHBCFG_HBSTLEN_2;
    cfg->instance->GAHBCFG |= USB_OTG_GAHBCFG_DMAEN;
  }
}

static void USB_EnableGlobalInt(USB_OTG_GlobalTypeDef *instance)
{
  instance->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}

static void USB_SetDevSpeed(USB_OTG_GlobalTypeDef *instance, unsigned int speed)
{
  USBx_DEVICE(instance)->DCFG |= speed;
}

static void USB_FlushTxFifo(USB_OTG_GlobalTypeDef *instance, unsigned int num)
{
  unsigned int count = 0;

  instance->GRSTCTL = ( USB_OTG_GRSTCTL_TXFFLSH | ( num << 6));

  do
  {
    if (++count > 200000)
    {
      while (1);
    }
  }
  while ((instance->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) == USB_OTG_GRSTCTL_TXFFLSH);
}

static void USB_FlushRxFifo(USB_OTG_GlobalTypeDef *instance)
{
  unsigned int count = 0;

  instance->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;

  do
  {
    if (++count > 200000)
    {
      while(1);
    }
  }
  while ((instance->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) == USB_OTG_GRSTCTL_RXFFLSH);
}

static void USB_DevInit(const USB_OTG_CfgTypeDef *cfg)
{
  unsigned int i;

#ifdef STM32F7
  /*Activate VBUS Sensing B */
  cfg->instance->GCCFG |= USB_OTG_GCCFG_VBDEN;

  if (cfg->vbus_sensing_enable == 0)
  {
    /* Deactivate VBUS Sensing B */
    cfg->instance->GCCFG &= ~ USB_OTG_GCCFG_VBDEN;

    /* B-peripheral session valid override enable*/
    cfg->instance->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
    cfg->instance->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
  }
#else
#ifdef STM32F2
  if (cfg->vbus_sensing_enable == 0)
  {
    /*
     * disable HW VBUS sensing. VBUS is internally considered to be always
     * at VBUS-Valid level (5V).
     */
    cfg->instance->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
    cfg->instance->GCCFG &= ~USB_OTG_GCCFG_VBUSBSEN;
    cfg->instance->GCCFG &= ~USB_OTG_GCCFG_VBUSASEN;
  }
  else
  {
    /* Enable HW VBUS sensing */
    cfg->instance->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
    cfg->instance->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
  }
#else
#ifdef STM32F4
  if (cfg->vbus_sensing_enable == 0)
  {
    /*
     * disable HW VBUS sensing. VBUS is internally considered to be always
     * at VBUS-Valid level (5V).
     */
    USBx_DEVICE->DCTL |= USB_OTG_DCTL_SDIS;
    cfg->instance->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
    cfg->instance->GCCFG &= ~USB_OTG_GCCFG_VBUSBSEN;
    cfg->instance->GCCFG &= ~USB_OTG_GCCFG_VBUSASEN;
  }
  else
  {
    /* Enable HW VBUS sensing */
    cfg->instance->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
    cfg->instance->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
  }
#else
  #error PLEASE DEFINE CPU FAMILY
#endif
#endif
#endif

  /* Restart the Phy Clock */
  USBx_PCGCCTL(cfg->instance) = 0;

  /* Device mode configuration */
  USBx_DEVICE(cfg->instance)->DCFG |= DCFG_FRAME_INTERVAL_80;

  if (cfg->instance == USB_OTG_HS)
  {
    /* Set High speed phy */
    USB_SetDevSpeed(cfg->instance, USB_OTG_SPEED_HIGH);
  }
  else
  {
    /* Set Full speed phy */
    USB_SetDevSpeed(cfg->instance, USB_OTG_SPEED_FULL);
  }

  /* Flush the FIFOs */
  USB_FlushTxFifo(cfg->instance, USB_MAX_ENDPOINTS); /* all Tx FIFOs */
  USB_FlushRxFifo(cfg->instance);

  /* Clear all pending Device Interrupts */
  USBx_DEVICE(cfg->instance)->DIEPMSK = 0;
  USBx_DEVICE(cfg->instance)->DOEPMSK = 0;
  USBx_DEVICE(cfg->instance)->DAINT = 0xFFFFFFFF;
  USBx_DEVICE(cfg->instance)->DAINTMSK = 0;

  for (i = 0; i < cfg->dev_endpoints; i++)
  {
    // if endpoint is enabled
    if ((USBx_INEP(cfg->instance, i)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == USB_OTG_DIEPCTL_EPENA)
    {
      //disable it and set the NAK bit
      USBx_INEP(cfg->instance, i)->DIEPCTL = USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK;
    }
    else
    {
      USBx_INEP(cfg->instance, i)->DIEPCTL = 0;
    }

    USBx_INEP(cfg->instance, i)->DIEPTSIZ = 0;
    USBx_INEP(cfg->instance, i)->DIEPINT  = 0xFF;
  }

  for (i = 0; i < cfg->dev_endpoints; i++)
  {
    if ((USBx_OUTEP(cfg->instance, i)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA)
    {
      USBx_OUTEP(cfg->instance, i)->DOEPCTL = USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK;
    }
    else
    {
      USBx_OUTEP(cfg->instance, i)->DOEPCTL = 0;
    }

    USBx_OUTEP(cfg->instance, i)->DOEPTSIZ = 0;
    USBx_OUTEP(cfg->instance, i)->DOEPINT  = 0xFF;
  }

  // mask FIFO underrun interrupt
  USBx_DEVICE(cfg->instance)->DIEPMSK &= ~USB_OTG_DIEPMSK_TXFURM;

  if (cfg->dma_enable)
  {
    /*Set threshold parameters */
    USBx_DEVICE(cfg->instance)->DTHRCTL = USB_OTG_DTHRCTL_TXTHRLEN_6 | USB_OTG_DTHRCTL_RXTHRLEN_6;
    USBx_DEVICE(cfg->instance)->DTHRCTL |= USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN | USB_OTG_DTHRCTL_NONISOTHREN;

    i = USBx_DEVICE(cfg->instance)->DTHRCTL;
  }

  /* Disable all interrupts. */
  cfg->instance->GINTMSK = 0;

  /* Clear any pending interrupts */
  cfg->instance->GINTSTS = 0xBFFFFFFF;

  /* Enable the common interrupts */
  if (!cfg->dma_enable)
  {
    cfg->instance->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
  }

  /* Enable interrupts matching to the Device mode ONLY */
  cfg->instance->GINTMSK |= USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST |
                    USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |
                    USB_OTG_GINTMSK_OEPINT   | USB_OTG_GINTMSK_IISOIXFRM|
                    USB_OTG_GINTMSK_PXFRM_IISOOXFRM | USB_OTG_GINTMSK_WUIM;

  if(cfg->Sof_enable)
  {
    cfg->instance->GINTMSK |= USB_OTG_GINTMSK_SOFM;
  }

  if (cfg->vbus_sensing_enable == ENABLE)
  {
    cfg->instance->GINTMSK |= (USB_OTG_GINTMSK_SRQIM | USB_OTG_GINTMSK_OTGINT);
  }
}

void USB_SetRxFiFo(USB_OTG_GlobalTypeDef *instance, unsigned int size)
{
  instance->GRXFSIZ = size;
}

void USB_SetTxFiFo(USB_OTG_GlobalTypeDef *instance, unsigned int fifo, unsigned int size)
{
  unsigned int i = 0;
  unsigned int Tx_Offset;

  /*  TXn min size = 16 words. (n  : Transmit FIFO index)
      When a TxFIFO is not used, the Configuration should be as follows:
          case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txm can use the space allocated for Txn.
         case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
         --> Txn should be configured with the minimum space of 16 words
     The FIFO is used optimally when used TxFIFOs are allocated in the top
         of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
     When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */

  Tx_Offset = instance->GRXFSIZ;

  if (!fifo)
  {
    instance->DIEPTXF0_HNPTXFSIZ = (size << 16) | Tx_Offset;
  }
  else
  {
    Tx_Offset += (instance->DIEPTXF0_HNPTXFSIZ) >> 16;
    for (i = 0; i < (fifo - 1); i++)
    {
      Tx_Offset += (instance->DIEPTXF[i] >> 16);
    }

    /* Multiply Tx_Size by 2 to get higher performance */
    instance->DIEPTXF[fifo - 1] = (size << 16) | Tx_Offset;
  }
}

static void USB_FIFO_Init(const USB_OTG_CfgTypeDef *cfg)
{
  unsigned int i;

  USB_SetRxFiFo(cfg->instance, 0x40);
  USB_SetTxFiFo(cfg->instance, 0, 0x40);
  for (i = 1; i < cfg->dev_endpoints; i++)
    USB_SetTxFiFo(cfg->instance, i, 0x40);
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling the pull-up/pull-down
  */
static void USB_DevConnect(USB_OTG_GlobalTypeDef *instance)
{
  USBx_DEVICE(instance)->DCTL &= ~USB_OTG_DCTL_SDIS;
  delayms(3);
}

int USBDeviceOtgInit(USB_OTG_CfgTypeDef *cfg, unsigned int dev_endpoints, unsigned int *xfer_buffers,
                      int xfer_buffers_length, int *endpoint_packet_lengths)
{
  cfg->dev_endpoints = dev_endpoints;

  for (int i = 0; i < dev_endpoints; i++)
  {
    cfg->in_xfer_buffers[i].buffer = (unsigned char*)xfer_buffers;
    int l = (*endpoint_packet_lengths + 3) / 4;
    xfer_buffers += l;
    cfg->out_xfer_buffers[i].buffer = (unsigned char*)xfer_buffers;
    xfer_buffers += l;
    xfer_buffers_length -= l;
  }
  if (xfer_buffers_length < 0)
    return 1;

  USB_CoreInit(cfg);

  cfg->instance->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
  delayms(50);

  /* Init Device */
  USB_DevInit(cfg);

  USB_FIFO_Init(cfg);

  USB_DevConnect(cfg->instance);

  USB_EnableGlobalInt(cfg->instance);

  return 0;
}

/**
  * @brief  USB_EPSetStall : set a stall condition over an EP
  * @param  num endpoint number
  * @param  is_in endpoint direction
  * @retval none
  */
static void USB_EPSetStall(USB_OTG_GlobalTypeDef *instance, unsigned int is_in, unsigned int num)
{
  if (is_in)
  {
    if (((USBx_INEP(instance, num)->DIEPCTL) & USB_OTG_DIEPCTL_EPENA) == 0)
    {
      USBx_INEP(instance, num)->DIEPCTL &= ~(USB_OTG_DIEPCTL_EPDIS);
    }
    USBx_INEP(instance, num)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
  }
  else
  {
    if (((USBx_OUTEP(instance, num)->DOEPCTL) & USB_OTG_DOEPCTL_EPENA) == 0)
    {
      USBx_OUTEP(instance, num)->DOEPCTL &= ~(USB_OTG_DOEPCTL_EPDIS);
    }
    USBx_OUTEP(instance, num)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
  }
}

void USBStallEndpoint(void *data, unsigned int endpoint)
{
  const USB_OTG_CfgTypeDef *cfg = data;
  USB_EPSetStall(cfg->instance, 1, endpoint);
  USB_EPSetStall(cfg->instance, 0, endpoint);
}

void *USBGetEndpointInBuffer(void *data, unsigned int endpoint)
{
  const USB_OTG_CfgTypeDef *cfg = data;
  return cfg->in_xfer_buffers[endpoint].buffer;
}

void USBSetAddress(void *data, unsigned short address)
{
  const USB_OTG_CfgTypeDef *cfg = data;
  USBx_DEVICE(cfg->instance)->DCFG &= ~ (USB_OTG_DCFG_DAD);
  USBx_DEVICE(cfg->instance)->DCFG |= (address << 4) & USB_OTG_DCFG_DAD ;
}

/**
  * @brief  USB_WritePacket : Writes a packet into the Tx FIFO associated
  *         with the EP/channel
  * @param  src :  pointer to source buffer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to write
  * @retval none
  */
void USB_WritePacket(const USB_OTG_CfgTypeDef *cfg, unsigned char *src, unsigned int ch_ep_num, unsigned int len)
{
  unsigned int count32b, i;

  if (!cfg->dma_enable)
  {
    count32b = (len + 3) / 4;
    for (i = 0; i < count32b; i++, src += 4)
      USBx_DFIFO(cfg->instance, ch_ep_num) = *(unsigned int*)src;
  }
}

void USBActivateEndpoint(void *data, unsigned int endpoint, unsigned int length)
{
  const USB_OTG_CfgTypeDef *cfg = data;
  USB_OTG_GlobalTypeDef *instance = cfg->instance;
  int iso = ((USBx_INEP(cfg->instance, endpoint)->DIEPCTL >> 18) & 3) == usb_endpoint_transfer_type_isochronous;

  /* Zero Length Packet? */
  unsigned int temp = USBx_INEP(instance, endpoint)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT | USB_OTG_DIEPTSIZ_XFRSIZ);
  if (!length)
    USBx_INEP(instance, endpoint)->DIEPTSIZ = temp | (USB_OTG_DIEPTSIZ_PKTCNT & (1 << 19));
  else
  {
    /* Program the transfer size and packet count
    * as follows: xfersize = N * maxpacket +
    * short_packet pktcnt = N + (short_packet
    * exist ? 1 : 0)
    */
    USBx_INEP(instance, endpoint)->DIEPTSIZ = temp
      | (USB_OTG_DIEPTSIZ_PKTCNT & (((length + USB_FS_MAX_PACKET_SIZE -1)/ USB_FS_MAX_PACKET_SIZE) << 19))
      | (USB_OTG_DIEPTSIZ_XFRSIZ & length);

    //todo
    if (iso)
    {
      USBx_INEP(instance, endpoint)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT);
      USBx_INEP(instance, endpoint)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1 << 29));
    }
  }

  if (cfg->dma_enable)
  {
    //todo
    //USBx_INEP(num)->DIEPDMA = ep->dma_addr;
  }
  else
  {
    if (!iso)
    {
      /* Enable the Tx FIFO Empty Interrupt for this EP */
      if (length)
        USBx_DEVICE(instance)->DIEPEMPMSK |= 1 << endpoint;
    }
  }

  if (iso)
  {
    if ((USBx_DEVICE(instance)->DSTS & ( 1 << 8 )) == 0)
    {
      USBx_INEP(instance, endpoint)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
    }
    else
    {
      USBx_INEP(instance, endpoint)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
    }
  }

  /* EP enable, IN data in FIFO */
  USBx_INEP(instance, endpoint)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);

  //if (iso)
  //{
    USB_WritePacket(cfg, cfg->in_xfer_buffers[endpoint].buffer, endpoint, length);
  //}

  /* Prepare endpoint for premature end of transfer */
  USB_EP_Receive (0,
                      NULL,
                      0);
}

void USBEnableEndpoint(void *data, unsigned int endpoint, USBEndpointDirection direction)
{
  const USB_OTG_CfgTypeDef *cfg = data;

  unsigned int ep_int_pos = 1 << endpoint;

  if (direction == usb_endpoint_direction_in || direction == usb_endpoint_direction_inout)
  {
    USBx_DEVICE(cfg->instance)->DAINTMSK |= USB_OTG_DAINTMSK_IEPM & ep_int_pos;
    USBx_INEP(cfg->instance, endpoint)->DIEPCTL |= USB_OTG_DIEPCTL_USBAEP;
  }

  if (direction == usb_endpoint_direction_out || direction == usb_endpoint_direction_inout)
  {
    USBx_DEVICE(cfg->instance)->DAINTMSK |= USB_OTG_DAINTMSK_OEPM & (ep_int_pos << 16);
    USBx_OUTEP(cfg->instance, endpoint)->DOEPCTL |= USB_OTG_DOEPCTL_USBAEP;
  }
}


void USBInitEndpoint(void *data, unsigned int endpoint, unsigned int max_packet_size,
                      USBEndpointTransferType transfer_type)
{
  const USB_OTG_CfgTypeDef *cfg = data;

  unsigned int temp = USBx_INEP(cfg->instance, endpoint)->DIEPCTL & ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP);
  temp |= (max_packet_size & USB_OTG_DIEPCTL_MPSIZ) | (transfer_type << 18) | (endpoint << 22) |
          USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
  USBx_INEP(cfg->instance, endpoint)->DIEPCTL = temp;

  temp = USBx_OUTEP(cfg->instance, endpoint)->DOEPCTL & ~(USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPTYP);
  temp |= (max_packet_size & USB_OTG_DOEPCTL_MPSIZ) | (transfer_type << 18) |
          USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
  USBx_OUTEP(cfg->instance, endpoint)->DOEPCTL = temp;
}

/**
  * @brief  USB_ReadInterrupts: return the global USB interrupt status
  * @retval interrupt status
  */
static unsigned int USB_ReadInterrupts(USB_OTG_GlobalTypeDef *instance)
{
  unsigned int v = instance->GINTSTS;
  v &= instance->GINTMSK;
  return v;
}

/**
  * @brief  USB_ReadDevAllOutEpInterrupt: return the USB device OUT endpoints interrupt status
  * @retval status
  */
static unsigned int USB_ReadDevAllOutEpInterrupt(USB_OTG_GlobalTypeDef *instance)
{
  unsigned int v = USBx_DEVICE(instance)->DAINT;
  v &= USBx_DEVICE(instance)->DAINTMSK;
  return ((v & 0xffff0000) >> 16);
}

/**
  * @brief  Returns Device OUT EP Interrupt register
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device OUT EP Interrupt register
  */
static unsigned int USB_ReadDevOutEPInterrupt(USB_OTG_GlobalTypeDef *instance, unsigned int epnum)
{
  unsigned int v = USBx_OUTEP(instance, epnum)->DOEPINT;
  v &= USBx_DEVICE(instance)->DOEPMSK;
  return v;
}

static void OEPINTHandler(USBDevice *device, USB_OTG_CfgTypeDef *cfg)
{
  unsigned int epnum, ep_intr, epint;

  epnum = 0;

  /* Read in the device interrupt bits */
  ep_intr = USB_ReadDevAllOutEpInterrupt(cfg->instance);

  while (ep_intr)
  {
    if (ep_intr & 1)
    {
      epint = USB_ReadDevOutEPInterrupt(cfg->instance, epnum);

      if(( epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC)
      {
        CLEAR_OUT_EP_INTR(cfg->instance, epnum, USB_OTG_DOEPINT_XFRC);

        /*if(cfg->dma_enable)
        {
          USBHandle.OUT_ep[epnum].xfer_count = USB_FS_MAX_PACKET_SIZE - (USBx_OUTEP(cfg->instance, epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ);
          USBHandle.OUT_ep[epnum].xfer_buff += USB_FS_MAX_PACKET_SIZE;
        }*/

        USBDeviceDataPacketReceived(device, epnum, cfg->out_xfer_buffers[epnum].buffer, cfg->out_xfer_buffers[epnum].xfer_len);
        /*if(cfg->dma_enable)
        {
          if((epnum == 0) && (USBHandle.OUT_ep[epnum].xfer_len == 0))
          {
            // this is ZLP, so prepare EP0 for next setup
            USB_EP0_OutStart();
          }
        }*/
      }

      if(( epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP)
      {
        // Inform the upper layer that a setup packet is available
        USBDeviceSetupPacketReceived(device, cfg->out_xfer_buffers[0].buffer, cfg->out_xfer_buffers[0].xfer_len);
        CLEAR_OUT_EP_INTR(cfg->instance, epnum, USB_OTG_DOEPINT_STUP);
      }

      if(( epint & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS)
      {
        CLEAR_OUT_EP_INTR(cfg->instance, epnum, USB_OTG_DOEPINT_OTEPDIS);
      }
      // Clear Status Phase Received interrupt
      if(( epint & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR)
      {
        CLEAR_OUT_EP_INTR(cfg->instance, epnum, USB_OTG_DOEPINT_OTEPSPR);
      }
    }
    epnum++;
    ep_intr >>= 1;
  }
}

/**
  * @brief  USB_ReadDevAllInEpInterrupt: return the USB device IN endpoints interrupt status
  * @retval status
  */
static unsigned int USB_ReadDevAllInEpInterrupt (USB_OTG_GlobalTypeDef *instance)
{
  unsigned int v;
  v  = USBx_DEVICE(instance)->DAINT;
  v &= USBx_DEVICE(instance)->DAINTMSK;
  return v & 0xFFFF;
}

/**
  * @brief  Returns Device IN EP Interrupt register
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device IN EP Interrupt register
  */
static unsigned int USB_ReadDevInEPInterrupt (USB_OTG_GlobalTypeDef *instance, unsigned int epnum)
{
  unsigned int v, msk, emp;

  msk = USBx_DEVICE(instance)->DIEPMSK;
  emp = USBx_DEVICE(instance)->DIEPEMPMSK;
  msk |= ((emp >> epnum) & 0x1) << 7;
  v = USBx_INEP(instance, epnum)->DIEPINT & msk;
  return v;
}

/**
  * @brief  Check FIFO for the next packet to be loaded.
  * @param  epnum : endpoint number
  * @retval none
  */
static void USB_WriteEmptyTxFifo(USB_OTG_GlobalTypeDef *instance, unsigned int epnum)
{
  unsigned int fifoemptymsk;
  /*USB_OTG_EPTypeDef *ep;
  unsigned int len;
  unsigned int len32b;

  ep = &USBHandle.IN_ep[epnum];
  len = ep->xfer_len - ep->xfer_count;

  if (len > USB_FS_MAX_PACKET_SIZE)
  {
    len = USB_FS_MAX_PACKET_SIZE;
  }


  len32b = (len + 3) / 4;

  while  ( (USBx_INEP(instance, epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) > len32b &&
           ep->xfer_count < ep->xfer_len &&
           ep->xfer_len != 0)
  {
    // Write the FIFO
    len = ep->xfer_len - ep->xfer_count;

    if (len > USB_FS_MAX_PACKET_SIZE)
    {
      len = USB_FS_MAX_PACKET_SIZE;
    }
    len32b = (len + 3) / 4;

    USB_WritePacket(ep->xfer_buff, epnum, len);

    ep->xfer_buff  += len;
    ep->xfer_count += len;
  }

  if(len <= 0)
  {*/
    fifoemptymsk = 0x1 << epnum;
    USBx_DEVICE(instance)->DIEPEMPMSK &= ~fifoemptymsk;
  //}
}

static void IEPINTHandler(USBDevice *device, const USB_OTG_CfgTypeDef *cfg)
{
  unsigned int epnum, ep_intr, epint, fifoemptymsk;

  epnum = 0;

  /* Read in the device interrupt bits */
  ep_intr = USB_ReadDevAllInEpInterrupt(cfg->instance);

  while ( ep_intr )
  {
    if (ep_intr & 0x1) /* In ITR */
    {
      epint = USB_ReadDevInEPInterrupt(cfg->instance, epnum);

      if(( epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC)
      {
        fifoemptymsk = 0x1 << epnum;
        USBx_DEVICE(cfg->instance)->DIEPEMPMSK &= ~fifoemptymsk;

        CLEAR_IN_EP_INTR(cfg->instance, epnum, USB_OTG_DIEPINT_XFRC);

        /*if (cfg->dma_enable)
        {
          USBHandle.IN_ep[epnum].xfer_buff += USB_FS_MAX_PACKET_SIZE;
        }*/

        USBDeviceContinueTransfer(device, epnum);

        /*if (cfg->dma_enable)
        {
          // this is ZLP, so prepare EP0 for next setup
          if((epnum == 0) && (USBHandle.IN_ep[epnum].xfer_len == 0))
          {
            // prepare to rx more setup packets
            USB_EP0_OutStart();
          }
        }*/
      }
      if(( epint & USB_OTG_DIEPINT_TOC) == USB_OTG_DIEPINT_TOC)
      {
        CLEAR_IN_EP_INTR(cfg->instance, epnum, USB_OTG_DIEPINT_TOC);
      }
      if(( epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE)
      {
        CLEAR_IN_EP_INTR(cfg->instance, epnum, USB_OTG_DIEPINT_ITTXFE);
      }
      if(( epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE)
      {
        CLEAR_IN_EP_INTR(cfg->instance, epnum, USB_OTG_DIEPINT_INEPNE);
      }
      if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
      {
        CLEAR_IN_EP_INTR(cfg->instance, epnum, USB_OTG_DIEPINT_EPDISD);
      }
      if(( epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE)
      {
        //todo
        //USBDeviceContinueTransfer(device, epnum);
        USB_WriteEmptyTxFifo(cfg->instance, epnum);
      }
    }
    epnum++;
    ep_intr >>= 1;
  }
}

/**
  * @brief  USB_ReadPacket : read a packet from the Tx FIFO associated
  *         with the EP/channel
  * @param  dest : destination pointer
  * @param  len : Number of bytes to read
  * @retval pointer to destination buffer
  */
void *USB_ReadPacket(USB_OTG_GlobalTypeDef *instance, unsigned char *dest, unsigned int len)
{
  unsigned int i;
  unsigned int count32b = (len + 3) / 4;

  for ( i = 0; i < count32b; i++, dest += 4 )
  {
    *((unsigned int*)dest) = USBx_DFIFO(instance, 0);
  }

  return dest;
}

/**
  * @brief  Prepare the EP0 to start the first control setup
  * @retval none
  */
void USB_EP0_OutStart(const USB_OTG_CfgTypeDef *cfg, USB_OTG_GlobalTypeDef *instance)
{
  USBx_OUTEP(instance, 0)->DOEPTSIZ = 0;
  USBx_OUTEP(instance, 0)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1 << 19)) ;
  USBx_OUTEP(instance, 0)->DOEPTSIZ |= (3 * 8);
  USBx_OUTEP(instance, 0)->DOEPTSIZ |=  USB_OTG_DOEPTSIZ_STUPCNT;

  if (cfg->dma_enable)
  {
    USBx_OUTEP(instance, 0)->DOEPDMA = (unsigned int)cfg->out_xfer_buffers[0].buffer;
    /* EP enable */
    USBx_OUTEP(instance, 0)->DOEPCTL = 0x80008000;
  }
}

static void USBRSTHandler(const USB_OTG_CfgTypeDef *cfg, USB_OTG_GlobalTypeDef *instance)
{
  unsigned int i;

  USBx_DEVICE(instance)->DCTL &= ~USB_OTG_DCTL_RWUSIG;
  USB_FlushTxFifo(instance, 0);

  for (i = 0; i < cfg->dev_endpoints; i++)
  {
    USBx_INEP(instance, i)->DIEPINT = 0xFF;
    USBx_OUTEP(instance, i)->DOEPINT = 0xFF;
  }
  USBx_DEVICE(instance)->DAINT = 0xFFFFFFFF;
  USBx_DEVICE(instance)->DAINTMSK |= 0x10001;

  if(cfg->use_dedicated_ep1)
  {
    USBx_DEVICE(instance)->DOUTEP1MSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
    USBx_DEVICE(instance)->DINEP1MSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
  }
  else
  {
    USBx_DEVICE(instance)->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM | USB_OTG_DOEPMSK_OTEPSPRM);
    USBx_DEVICE(instance)->DIEPMSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
  }

  /* Set Default Address to 0 */
  USBx_DEVICE(instance)->DCFG &= ~USB_OTG_DCFG_DAD;

  /* setup EP0 to receive SETUP packets */
  USB_EP0_OutStart(cfg, instance);

  instance->GINTSTS = USB_OTG_GINTSTS_USBRST;
}

void USBDeviceInterruptHandler(int device_no)
{
  USBDevice *device = &usb_devices[device_no];
  USB_OTG_CfgTypeDef *cfg = device->data;
  USB_OTG_GlobalTypeDef *instance = cfg->instance;
  unsigned int status = USB_ReadInterrupts(instance);

  if (status == 0)
    return;

  if (status & USB_OTG_GINTSTS_MMIS)
  {
    /* incorrect mode, acknowledge the interrupt */
    instance->GINTSTS = USB_OTG_GINTSTS_MMIS;
  }

  if (status & USB_OTG_GINTSTS_OEPINT)
    OEPINTHandler(device, cfg);

  if (status & USB_OTG_GINTSTS_IEPINT)
    IEPINTHandler(device, cfg);

  /* Handle Resume Interrupt */
  if (status & USB_OTG_GINTSTS_WKUINT)
  {
    /* Clear the Remote Wake-up Signaling */
    USBx_DEVICE(instance)->DCTL &= ~USB_OTG_DCTL_RWUSIG;

    if (device->config->Resume_Callback)
      device->config->Resume_Callback(device_no);

    instance->GINTSTS = USB_OTG_GINTSTS_WKUINT;
  }

  /* Handle Suspend Interrupt */
  if (status & USB_OTG_GINTSTS_USBSUSP)
  {
    if((USBx_DEVICE(instance)->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
    {
      if (device->config->Suspend_Callback)
        device->config->Suspend_Callback(device_no);
    }
    instance->GINTSTS = USB_OTG_GINTSTS_USBSUSP;
  }

#if !defined(STM32F2) && !defined(STM32F4)
  /* Handle LPM Interrupt */
  if (status & USB_OTG_GINTSTS_LPMINT)
  {
    instance->GINTSTS = USB_OTG_GINTSTS_LPMINT;

    USBDeviceCallLPMCallbacks(device_no, device);
  }
#endif

  /* Handle Reset Interrupt */
  if (status & USB_OTG_GINTSTS_USBRST)
    USBRSTHandler(cfg, instance);

  if (status & USB_OTG_GINTSTS_ENUMDNE)
  {
    USBDeviceActivateSetup(device_no, device);

    instance->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;
    unsigned int trdt = instance == USB_OTG_FS ? 6 : 9;
    instance->GUSBCFG |= trdt << 10;

    instance->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
  }

  /* Handle RxQLevel Interrupt */
  if (status & USB_OTG_GINTSTS_RXFLVL)
  {
    USB_MASK_INTERRUPT(instance, USB_OTG_GINTSTS_RXFLVL);

    unsigned int temp = instance->GRXSTSP;
    unsigned int type = (temp & USB_OTG_GRXSTSP_PKTSTS) >> 17;
    unsigned int size = (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
    unsigned int ep_no = temp & USB_OTG_GRXSTSP_EPNUM;
    USB_OTG_XferBuffer *xfer_buff = &cfg->out_xfer_buffers[ep_no];

    if (type == STS_SETUP_UPDT || type == STS_DATA_UPDT)
    {
      USB_ReadPacket(cfg->instance, xfer_buff->buffer, size);
      xfer_buff->xfer_len = size;
    }

    USB_UNMASK_INTERRUPT(instance, USB_OTG_GINTSTS_RXFLVL);
  }

  /* Handle SOF Interrupt */
  if(status & USB_OTG_GINTSTS_SOF)
  {
    USBDeviceCallSofCallbacks(device_no, device);
    instance->GINTSTS = USB_OTG_GINTSTS_SOF;
  }

  /* Handle Incomplete ISO IN Interrupt */
  if(status & USB_OTG_GINTSTS_IISOIXFR)
  {
    //TODO: epnum?
    //USBHandle.Callbacks->ISOINIncomplete_Callback();
    instance->GINTSTS = USB_OTG_GINTSTS_IISOIXFR;
  }

  /* Handle Incomplete ISO OUT Interrupt */
  if(status & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT)
  {
    //TODO: epnum?
    //USBHandle.Callbacks->ISOOUTIncomplete_Callback();
    instance->GINTSTS = USB_OTG_GINTSTS_PXFR_INCOMPISOOUT;
  }

  /* Handle Connection event Interrupt */
  if(status & USB_OTG_GINTSTS_SRQINT)
  {
    if (device->config->Connect_Callback)
      device->config->Connect_Callback(device_no);
    instance->GINTSTS = USB_OTG_GINTSTS_SRQINT;
  }

  /* Handle Disconnection event Interrupt */
  if(status & USB_OTG_GINTSTS_OTGINT)
  {
    unsigned int temp = instance->GOTGINT;

    if ((temp & USB_OTG_GOTGINT_SEDET) && device->config->Disconnect_Callback)
      device->config->Disconnect_Callback(device_no);
    instance->GOTGINT |= temp;
  }
}