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

typedef struct
{
  unsigned char xfer_buff[USB_DEVICE_MAX_PACKET_SIZE];     /*!< Pointer to transfer buffer                                               */
} USB_OTG_EPTypeDef;

const char otg_endpoints[USB_MAX_ENDPOINTS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

typedef struct
{
  USB_OTG_EPTypeDef in_endpoints[USB_MAX_ENDPOINTS];
  USB_OTG_EPTypeDef out_endpoints[USB_MAX_ENDPOINTS];
} USB_OTG_Data;

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

static void USB_DevInit(const USB_OTG_CfgTypeDef *cfg, unsigned int dev_endpoints)
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

  for (i = 0; i < dev_endpoints; i++)
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

  for (i = 0; i < dev_endpoints; i++)
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

static void USB_FIFO_Init(const USB_OTG_CfgTypeDef *cfg, unsigned int dev_endpoints)
{
  unsigned int i;

  USB_SetRxFiFo(cfg->instance, 0x40);
  USB_SetTxFiFo(cfg->instance, 0, 0x40);
  for (i = 1; i < dev_endpoints; i++)
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

void USBDeviceOtgInit(const USB_OTG_CfgTypeDef *cfg, unsigned int dev_endpoints)
{
  USB_CoreInit(cfg);

  cfg->instance->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
  delayms(50);

  /* Init Device */
  USB_DevInit(cfg, dev_endpoints);

  USB_FIFO_Init(cfg, dev_endpoints);

  USB_DevConnect(cfg->instance);

  USB_EnableGlobalInt(cfg->instance);
}

void USBEnableEndpoint(void *data, unsigned int endpoint, USBEndpointDirection direction)
{
  //todo
}

void USBActivateEndpoint(void *data, unsigned int endpoint, unsigned int length)
{
  //todo
}

void USBStallEndpoint(void *data, unsigned int endpoint)
{
  //todo
}

void *USBGetEndpointInBuffer(void *data, int endpoint)
{
  //todo
  return NULL;
}

void *USBGetEndpointOutBuffer(void *data, int endpoint)
{
  //todo
  return NULL;
}

void USBSetAddress(void *data, unsigned short address)
{
  //todo
}

void USBSetEndpointTransferType(void *data, int endpoint, unsigned int max_packet_size, USBEndpointTransferType transfer_type)
{
  const USB_OTG_CfgTypeDef *cfg = data;
  unsigned int temp = USBx_INEP(cfg->instance, endpoint)->DIEPCTL & ~(USB_OTG_DIEPCTL_MPSIZ | USB_OTG_DIEPCTL_EPTYP);
  temp |= (max_packet_size & USB_OTG_DIEPCTL_MPSIZ) | (transfer_type << 18);
  USBx_INEP(cfg->instance, endpoint)->DIEPCTL = temp;

  temp = USBx_OUTEP(cfg->instance, endpoint)->DOEPCTL & ~(USB_OTG_DOEPCTL_MPSIZ | USB_OTG_DOEPCTL_EPTYP);
  temp |= (max_packet_size & USB_OTG_DOEPCTL_MPSIZ) | (transfer_type << 18);
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

static void OEPINTHandler(const USB_OTG_CfgTypeDef *cfg)
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

        if(cfg->dma_enable)
        {
          USBHandle.OUT_ep[epnum].xfer_count = USB_FS_MAX_PACKET_SIZE - (USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ);
          USBHandle.OUT_ep[epnum].xfer_buff += USB_FS_MAX_PACKET_SIZE;
        }

        USBHandle.Callbacks->DataOutStage_Callback(epnum);
        if(cfg->dma_enable)
        {
          if((epnum == 0) && (USBHandle.OUT_ep[epnum].xfer_len == 0))
          {
            // this is ZLP, so prepare EP0 for next setup
            USB_EP0_OutStart();
          }
        }
      }

      if(( epint & USB_OTG_DOEPINT_STUP) == USB_OTG_DOEPINT_STUP)
      {
        // Inform the upper layer that a setup packet is available
        USBHandle.Callbacks->SetupStage_Callback();
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
  USB_OTG_EPTypeDef *ep;
  unsigned int len;
  unsigned int len32b;
  unsigned int fifoemptymsk;

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
    /* Write the FIFO */
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
  {
    fifoemptymsk = 0x1 << epnum;
    USBx_DEVICE(instance)->DIEPEMPMSK &= ~fifoemptymsk;
  }
}

static void IEPINTHandler(const USB_OTG_CfgTypeDef *cfg)
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

        if (cfg->dma_enable)
        {
          USBHandle.IN_ep[epnum].xfer_buff += USB_FS_MAX_PACKET_SIZE;
        }

        USBHandle.Callbacks->DataInStage_Callback(epnum);

        if (cfg->dma_enable)
        {
          /* this is ZLP, so prepare EP0 for next setup */
          if((epnum == 0) && (USBHandle.IN_ep[epnum].xfer_len == 0))
          {
            /* prepare to rx more setup packets */
            USB_EP0_OutStart();
          }
        }
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
        USB_WriteEmptyTxFifo(cfg->instance, epnum);
      }
    }
    epnum++;
    ep_intr >>= 1;
  }
}

void USBDeviceInterruptHandler(int device_no)
{
  USBDevice *device = &usb_devices[device_no];
  const USB_OTG_CfgTypeDef *cfg = device->data;
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
  {
    OEPINTHandler(cfg);
  }

  if (status & USB_OTG_GINTSTS_IEPINT)
  {
    IEPINTHandler(cfg);
  }

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

    for (int i = 0; i < USB_DEVICE_MAX_CLASSES; i++)
    {
      usb_callback c = device->LPM_Callbacks[i];
      if (c)
        c(device_no);
      else
        break;
    }
  }
#endif

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
    unsigned int ep = &USBHandle.OUT_ep[temp & USB_OTG_GRXSTSP_EPNUM];

    if(((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_DATA_UPDT)
    {
      if((temp & USB_OTG_GRXSTSP_BCNT) != 0)
      {
        USB_ReadPacket(ep->xfer_buff, (temp & USB_OTG_GRXSTSP_BCNT) >> 4);
        ep->xfer_buff += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
        ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
      }
    }
    else if (((temp & USB_OTG_GRXSTSP_PKTSTS) >> 17) ==  STS_SETUP_UPDT)
    {
      USB_ReadPacket(USBHandle.Setup.data, 8);
      ep->xfer_count += (temp & USB_OTG_GRXSTSP_BCNT) >> 4;
    }

    USB_UNMASK_INTERRUPT(instance, USB_OTG_GINTSTS_RXFLVL);
  }

  /* Handle SOF Interrupt */
  if(status & USB_OTG_GINTSTS_SOF)
  {
    for (int i = 0; i < USB_DEVICE_MAX_CLASSES; i++)
    {
      usb_callback c = device->SOF_Callbacks[i];
      if (c)
        c(device_no);
      else
        break;
    }
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