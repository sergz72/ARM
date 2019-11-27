#include <usb.h>

#define DCFG_FRAME_INTERVAL_80                 0U
#define DCFG_FRAME_INTERVAL_85                 1U
#define DCFG_FRAME_INTERVAL_90                 2U
#define DCFG_FRAME_INTERVAL_95                 3U

#define USB_OTG_SPEED_HIGH                     0U
#define USB_OTG_SPEED_HIGH_IN_FULL             1U
#define USB_OTG_SPEED_LOW                      2U
#define USB_OTG_SPEED_FULL                     3U

#define DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ     (0 << 1)
#define DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ     (1 << 1)
#define DSTS_ENUMSPD_LS_PHY_6MHZ               (2 << 1)
#define DSTS_ENUMSPD_FS_PHY_48MHZ              (3 << 1)

#define USBx_PCGCCTL    *(__IO unsigned int*)((unsigned int)USBHandle.Instance + USB_OTG_PCGCCTL_BASE)
#define USBx_DFIFO(i)   *(__IO unsigned int*)((unsigned int)USBHandle.Instance + USB_OTG_FIFO_BASE + (i) * USB_OTG_FIFO_SIZE)

static void USB_EnableGlobalInt(void);
static void USB_DisableGlobalInt(void);
static void USB_CoreInit(void);
static void USB_DevInit(void);
static void USB_DevConnect(void);
static void USB_CoreReset(void);
static void USB_SetDevSpeed(unsigned int speed);
static void USB_FlushRxFifo(void);
static void USB_EPInit(void);

USB_HandleTypeDef USBHandle;

void USBInit(USB_OTG_GlobalTypeDef *USBx,
             const USB_OTG_CfgTypeDef *cfg,
             const USB_Callbacks *callbacks,
             const USB_ClassTypeDef *pClass,
             const USB_DescriptorsTypeDef *pDesc)
{
  unsigned int i;

  USBHandle.Instance = USBx;
  USBHandle.Cfg = (USB_OTG_CfgTypeDef *)cfg;
  USBHandle.Callbacks = (USB_Callbacks*)callbacks;
  USBHandle.pClass = (USB_ClassTypeDef *)pClass;
  USBHandle.pDesc = (USB_DescriptorsTypeDef *)pDesc;

  /* Set Device initial State */
  USBHandle.dev_state  = USB_STATE_DEFAULT;
  USBHandle.dev_old_state  = USB_STATE_DEFAULT;
  USBHandle.dev_address = 0;
  USBHandle.dev_remote_wakeup = cfg->dev_remote_wakeup;
  USBHandle.dev_test_mode = 0;

	USB_DisableGlobalInt();

	USB_CoreInit();

	// Set device mode
  USBHandle.Instance->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
  delayms(50);

  USB_EPInit();

  /* Init Device */
  USB_DevInit();

  USB_FIFO_Init();

  USB_DevConnect();
  USB_EnableGlobalInt();
}

/**
  * @brief  Set Rx FIFO
  * @param  hpcd: PCD handle
  * @param  size: Size of Rx fifo
  * @retval HAL status
  */
void USB_SetRxFiFo(unsigned int size)
{
  USBHandle.Instance->GRXFSIZ = size;
}

/**
  * @brief  Set Tx FIFO
  * @param  fifo: The number of Tx fifo
  * @param  size: Fifo size
  * @retval HAL status
  */
void USB_SetTxFiFo(unsigned int fifo, unsigned int size)
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

  Tx_Offset = USBHandle.Instance->GRXFSIZ;

  if (!fifo)
  {
    USBHandle.Instance->DIEPTXF0_HNPTXFSIZ = (size << 16) | Tx_Offset;
  }
  else
  {
    Tx_Offset += (USBHandle.Instance->DIEPTXF0_HNPTXFSIZ) >> 16;
    for (i = 0; i < (fifo - 1); i++)
    {
      Tx_Offset += (USBHandle.Instance->DIEPTXF[i] >> 16);
    }

    /* Multiply Tx_Size by 2 to get higher performance */
    USBHandle.Instance->DIEPTXF[fifo - 1] = (size << 16) | Tx_Offset;
  }
}

static void USB_EPInit(void)
{
  int i;

  /* Init endpoints structures */
  for (i = 0; i < USB_EP_MAX; i++)
  {
    /* Init ep structure */
    USBHandle.IN_ep[i].xfer_buff = 0;
    USBHandle.IN_ep[i].xfer_len = 0;
    USBHandle.IN_ep[i].xfer_count = 0;
    USBHandle.IN_ep[i].is_stall = 0;
    USBHandle.IN_ep[i].type = USB_EP_TYPE_CTRL;

    USBHandle.OUT_ep[i].xfer_buff = 0;
    USBHandle.OUT_ep[i].xfer_len = 0;
    USBHandle.OUT_ep[i].xfer_count = 0;
    USBHandle.OUT_ep[i].is_stall = 0;
    USBHandle.OUT_ep[i].type = USB_EP_TYPE_CTRL;

    USBHandle.Instance->DIEPTXF[i] = 0;
  }
}

static void USB_CoreInit(void)
{
#ifdef USB_FS
  /* Select FS Embedded PHY */
  USBHandle.Instance->GUSBCFG |= USB_OTG_GUSBCFG_PHYSEL;

  /* Reset after a PHY select and set Host mode */
  USB_CoreReset();

  /* Deactivate the power down*/
  USBHandle.Instance->GCCFG = USB_OTG_GCCFG_PWRDWN;
#else
  USBHandle.Instance->GCCFG &= ~(USB_OTG_GCCFG_PWRDWN);

  /* Init The ULPI Interface */
  USBHandle.Instance->GUSBCFG &= ~(USB_OTG_GUSBCFG_TSDPS | USB_OTG_GUSBCFG_ULPIFSLS | USB_OTG_GUSBCFG_PHYSEL);

  /* Select vbus source */
  USBHandle.Instance->GUSBCFG &= ~(USB_OTG_GUSBCFG_ULPIEVBUSD | USB_OTG_GUSBCFG_ULPIEVBUSI);
  if(USBHandle.Cfg->use_external_vbus)
  {
    USBHandle.Instance->GUSBCFG |= USB_OTG_GUSBCFG_ULPIEVBUSD;
  }
  /* Reset after a PHY select  */
  USB_CoreReset();
#endif

  if (USBHandle.Cfg->dma_enable)
  {
    USBHandle.Instance->GAHBCFG |= (USB_OTG_GAHBCFG_HBSTLEN_1 | USB_OTG_GAHBCFG_HBSTLEN_2);
    USBHandle.Instance->GAHBCFG |= USB_OTG_GAHBCFG_DMAEN;
  }
}

/**
  * @brief  USB_EnableGlobalInt
  *         Enables the controller's Global Int in the AHB Config reg
  */
static void USB_EnableGlobalInt(void)
{
  USBHandle.Instance->GAHBCFG |= USB_OTG_GAHBCFG_GINT;
}


/**
  * @brief  USB_DisableGlobalInt
  *         Disable the controller's Global Int in the AHB Config reg
*/
static void USB_DisableGlobalInt(void)
{
  USBHandle.Instance->GAHBCFG &= ~USB_OTG_GAHBCFG_GINT;
}

/**
  * @brief  Reset the USB Core (needed after USB clock settings change)
  */
static void USB_CoreReset(void)
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
  while ((USBHandle.Instance->GRSTCTL & USB_OTG_GRSTCTL_AHBIDL) == 0);
  
  /* Core Soft Reset */
  count = 0;
  USBHandle.Instance->GRSTCTL |= USB_OTG_GRSTCTL_CSRST;

  do
  {
    if (++count > 200000)
    {
      while (1);//timeout
    }
  }
  while ((USBHandle.Instance->GRSTCTL & USB_OTG_GRSTCTL_CSRST) == USB_OTG_GRSTCTL_CSRST);
}

/**
  * @brief  USB_DevInit : Initializes the USB_OTG controller registers 
  *         for device mode
  */
static void USB_DevInit(void)
{
  unsigned int i;

#ifdef STM32F7
  /*Activate VBUS Sensing B */
  USBHandle.Instance->GCCFG |= USB_OTG_GCCFG_VBDEN;
  
  if (USBHandle.Cfg->vbus_sensing_enable == 0)
  {
    /* Deactivate VBUS Sensing B */
    USBHandle.Instance->GCCFG &= ~ USB_OTG_GCCFG_VBDEN;
    
    /* B-peripheral session valid override enable*/
    USBHandle.Instance->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
    USBHandle.Instance->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
  }
#else
#ifdef STM32F2
  if (USBHandle.Cfg->vbus_sensing_enable == 0)
  {
    /*
     * disable HW VBUS sensing. VBUS is internally considered to be always
     * at VBUS-Valid level (5V).
     */
    USBHandle.Instance->GCCFG |= USB_OTG_GCCFG_NOVBUSSENS;
    USBHandle.Instance->GCCFG &= ~USB_OTG_GCCFG_VBUSBSEN;
    USBHandle.Instance->GCCFG &= ~USB_OTG_GCCFG_VBUSASEN;
  }
  else
  {
    /* Enable HW VBUS sensing */
    USBHandle.Instance->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
    USBHandle.Instance->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
  }
#else
  #error PLEASE DEFINE CPU FAMILY
#endif
#endif

  /* Restart the Phy Clock */
  USBx_PCGCCTL = 0;

  /* Device mode configuration */
  USBx_DEVICE->DCFG |= DCFG_FRAME_INTERVAL_80;

  if (USBHandle.Instance == USB_OTG_HS)
  {
    /* Set High speed phy */
    USB_SetDevSpeed(USB_OTG_SPEED_HIGH);
  }
  else
  {
    /* Set Full speed phy */
    USB_SetDevSpeed(USB_OTG_SPEED_FULL);
  }

  /* Flush the FIFOs */
  USB_FlushTxFifo(0x10); /* all Tx FIFOs */
  USB_FlushRxFifo();
  
  /* Clear all pending Device Interrupts */
  USBx_DEVICE->DIEPMSK = 0;
  USBx_DEVICE->DOEPMSK = 0;
  USBx_DEVICE->DAINT = 0xFFFFFFFF;
  USBx_DEVICE->DAINTMSK = 0;
  
  for (i = 0; i < USBHandle.Cfg->dev_endpoints; i++)
  {
    if ((USBx_INEP(i)->DIEPCTL & USB_OTG_DIEPCTL_EPENA) == USB_OTG_DIEPCTL_EPENA)
    {
      USBx_INEP(i)->DIEPCTL = (USB_OTG_DIEPCTL_EPDIS | USB_OTG_DIEPCTL_SNAK);
    }
    else
    {
      USBx_INEP(i)->DIEPCTL = 0;
    }
    
    USBx_INEP(i)->DIEPTSIZ = 0;
    USBx_INEP(i)->DIEPINT  = 0xFF;
  }
  
  for (i = 0; i < USBHandle.Cfg->dev_endpoints; i++)
  {
    if ((USBx_OUTEP(i)->DOEPCTL & USB_OTG_DOEPCTL_EPENA) == USB_OTG_DOEPCTL_EPENA)
    {
      USBx_OUTEP(i)->DOEPCTL = (USB_OTG_DOEPCTL_EPDIS | USB_OTG_DOEPCTL_SNAK);
    }
    else
    {
      USBx_OUTEP(i)->DOEPCTL = 0;
    }
    
    USBx_OUTEP(i)->DOEPTSIZ = 0;
    USBx_OUTEP(i)->DOEPINT  = 0xFF;
  }
  
  USBx_DEVICE->DIEPMSK &= ~(USB_OTG_DIEPMSK_TXFURM);
  
  if (USBHandle.Cfg->dma_enable)
  {
    /*Set threshold parameters */
    USBx_DEVICE->DTHRCTL = (USB_OTG_DTHRCTL_TXTHRLEN_6 | USB_OTG_DTHRCTL_RXTHRLEN_6);
    USBx_DEVICE->DTHRCTL |= (USB_OTG_DTHRCTL_RXTHREN | USB_OTG_DTHRCTL_ISOTHREN | USB_OTG_DTHRCTL_NONISOTHREN);
    
    i= USBx_DEVICE->DTHRCTL;
  }
  
  /* Disable all interrupts. */
  USBHandle.Instance->GINTMSK = 0;
  
  /* Clear any pending interrupts */
  USBHandle.Instance->GINTSTS = 0xBFFFFFFF;

  /* Enable the common interrupts */
  if (!USBHandle.Cfg->dma_enable)
  {
    USBHandle.Instance->GINTMSK |= USB_OTG_GINTMSK_RXFLVLM;
  }
  
  /* Enable interrupts matching to the Device mode ONLY */
  USBHandle.Instance->GINTMSK |= (USB_OTG_GINTMSK_USBSUSPM | USB_OTG_GINTMSK_USBRST |\
                    USB_OTG_GINTMSK_ENUMDNEM | USB_OTG_GINTMSK_IEPINT |\
                    USB_OTG_GINTMSK_OEPINT   | USB_OTG_GINTMSK_IISOIXFRM|\
                    USB_OTG_GINTMSK_PXFRM_IISOOXFRM | USB_OTG_GINTMSK_WUIM);
  
  if(USBHandle.Cfg->Sof_enable)
  {
    USBHandle.Instance->GINTMSK |= USB_OTG_GINTMSK_SOFM;
  }

  if (USBHandle.Cfg->vbus_sensing_enable == ENABLE)
  {
    USBHandle.Instance->GINTMSK |= (USB_OTG_GINTMSK_SRQIM | USB_OTG_GINTMSK_OTGINT);
  }
}

/**
  * @brief  USB_SetDevSpeed :Initializes the DevSpd field of DCFG register 
  *         depending the PHY type and the enumeration speed of the device.
  * @param  speed : device speed
  *          This parameter can be one of these values:
  *            @arg USB_OTG_SPEED_HIGH: High speed mode
  *            @arg USB_OTG_SPEED_HIGH_IN_FULL: High speed core in Full Speed mode
  *            @arg USB_OTG_SPEED_FULL: Full speed mode
  *            @arg USB_OTG_SPEED_LOW: Low speed mode
  */
static void USB_SetDevSpeed(unsigned int speed)
{
  USBx_DEVICE->DCFG |= speed;
}

/**
  * @brief  USB_OTG_FlushTxFifo : Flush a Tx FIFO
  * @param  num : FIFO number
  *         This parameter can be a value from 1 to 15
            15 means Flush all Tx FIFOs
  */
void USB_FlushTxFifo(unsigned int num)
{
  unsigned int count = 0;

  USBHandle.Instance->GRSTCTL = ( USB_OTG_GRSTCTL_TXFFLSH | ( num << 6));
 
  do
  {
    if (++count > 200000)
    {
      while (1);
    }
  }
  while ((USBHandle.Instance->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH) == USB_OTG_GRSTCTL_TXFFLSH);
}

/**
  * @brief  USB_FlushRxFifo : Flush Rx FIFO
  */
static void USB_FlushRxFifo(void)
{
  unsigned int count = 0;

  USBHandle.Instance->GRSTCTL = USB_OTG_GRSTCTL_RXFFLSH;
  
  do
  {
    if (++count > 200000)
    {
      while(1);
    }
  }
  while ((USBHandle.Instance->GRSTCTL & USB_OTG_GRSTCTL_RXFFLSH) == USB_OTG_GRSTCTL_RXFFLSH);
}

/**
  * @brief  USB_DevConnect : Connect the USB device by enabling the pull-up/pull-down
  */
static void USB_DevConnect(void)
{
  USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_SDIS;
  delayms(3);
}

/**
  * @brief  USB_ReadInterrupts: return the global USB interrupt status
  * @retval interrupt status
  */
unsigned int USB_ReadInterrupts(void)
{
  unsigned int v = USBHandle.Instance->GINTSTS;
  v &= USBHandle.Instance->GINTMSK;
  return v;  
}

/**
  * @brief  USB_ReadDevAllOutEpInterrupt: return the USB device OUT endpoints interrupt status
  * @retval status
  */
unsigned int USB_ReadDevAllOutEpInterrupt (void)
{
  unsigned int v = USBx_DEVICE->DAINT;
  v &= USBx_DEVICE->DAINTMSK;
  return ((v & 0xffff0000) >> 16);
}

/**
  * @brief  Activate EP0 for Setup transactions
  */
void USB_ActivateSetup (void)
{
  /* Set the MPS of the IN EP based on the enumeration speed */
  USBx_INEP(0)->DIEPCTL &= ~USB_OTG_DIEPCTL_MPSIZ;

  if((USBx_DEVICE->DSTS & USB_OTG_DSTS_ENUMSPD) == DSTS_ENUMSPD_LS_PHY_6MHZ)
  {
    USBx_INEP(0)->DIEPCTL |= 3;
  }
  USBx_DEVICE->DCTL |= USB_OTG_DCTL_CGINAK;
}

/**
  * @brief  Prepare the EP0 to start the first control setup
  * @retval none
  */
void USB_EP0_OutStart(void)
{
  USBx_OUTEP(0)->DOEPTSIZ = 0;
  USBx_OUTEP(0)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1 << 19)) ;
  USBx_OUTEP(0)->DOEPTSIZ |= (3 * 8);
  USBx_OUTEP(0)->DOEPTSIZ |=  USB_OTG_DOEPTSIZ_STUPCNT;

  if (USBHandle.Cfg->dma_enable)
  {
    USBx_OUTEP(0)->DOEPDMA = (unsigned int)USBHandle.Setup.data;
    /* EP enable */
    USBx_OUTEP(0)->DOEPCTL = 0x80008000;
  }
}

/**
  * @brief  Open and configure an endpoint.
  * @param  is_in: endpoint direction (in/out)
  * @param  num: endpoint number
  * @param  maxpacket: endpoint max packet size
  * @param  type: endpoint type
  * @retval none
  */
void USB_EP_Open(unsigned int is_in, unsigned int num, unsigned int type)
{
  if (is_in)
  {
    USBHandle.IN_ep[num].type = type;
    USBx_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_IEPM & (1 << num);

    if (((USBx_INEP(num)->DIEPCTL) & USB_OTG_DIEPCTL_USBAEP) == 0)
    {
      USBx_INEP(num)->DIEPCTL |= ((USB_OTG_FS_MAX_PACKET_SIZE & USB_OTG_DIEPCTL_MPSIZ ) | (type << 18 ) |
        (num << 22 ) | (USB_OTG_DIEPCTL_SD0PID_SEVNFRM) | (USB_OTG_DIEPCTL_USBAEP));
    }

  }
  else
  {
    USBHandle.OUT_ep[num].type = type;
    USBx_DEVICE->DAINTMSK |= USB_OTG_DAINTMSK_OEPM & ((1 << num) << 16);

    if (((USBx_OUTEP(num)->DOEPCTL) & USB_OTG_DOEPCTL_USBAEP) == 0)
    {
      USBx_OUTEP(num)->DOEPCTL |= ((USB_OTG_FS_MAX_PACKET_SIZE & USB_OTG_DOEPCTL_MPSIZ ) | (type << 18 ) |
       (USB_OTG_DIEPCTL_SD0PID_SEVNFRM) | (USB_OTG_DOEPCTL_USBAEP));
    }
  }
}

/**
  * @brief  USB_ReadPacket : read a packet from the Tx FIFO associated
  *         with the EP/channel
  * @param  dest : destination pointer
  * @param  len : Number of bytes to read
  * @retval pointer to destination buffer
  */
void *USB_ReadPacket(unsigned char *dest, unsigned int len)
{
  unsigned int i;
  unsigned int count32b = (len + 3) / 4;

  for ( i = 0; i < count32b; i++, dest += 4 )
  {
    *((unsigned int*)dest) = USBx_DFIFO(0);
  }

  return (void*)dest;
}

/**
  * @brief  USB_WritePacket : Writes a packet into the Tx FIFO associated
  *         with the EP/channel
  * @param  src :  pointer to source buffer
  * @param  ch_ep_num : endpoint or host channel number
  * @param  len : Number of bytes to write
  * @retval none
  */
void USB_WritePacket(unsigned char *src, unsigned int ch_ep_num, unsigned int len)
{
  unsigned int count32b, i;

  if (!USBHandle.Cfg->dma_enable)
  {
    count32b = (len + 3) / 4;
    for (i = 0; i < count32b; i++, src += 4)
    {
      USBx_DFIFO(ch_ep_num) = *((unsigned int*)src);
    }
  }
}

/**
  * @brief  Returns Device OUT EP Interrupt register
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device OUT EP Interrupt register
  */
unsigned int USB_ReadDevOutEPInterrupt (unsigned int epnum)
{
  unsigned int v = USBx_OUTEP(epnum)->DOEPINT;
  v &= USBx_DEVICE->DOEPMSK;
  return v;
}

/**
  * @brief  Returns Device IN EP Interrupt register
  * @param  epnum : endpoint number
  *          This parameter can be a value from 0 to 15
  * @retval Device IN EP Interrupt register
  */
unsigned int USB_ReadDevInEPInterrupt (unsigned int epnum)
{
  unsigned int v, msk, emp;

  msk = USBx_DEVICE->DIEPMSK;
  emp = USBx_DEVICE->DIEPEMPMSK;
  msk |= ((emp >> epnum) & 0x1) << 7;
  v = USBx_INEP(epnum)->DIEPINT & msk;
  return v;
}

/**
  * @brief  Check FIFO for the next packet to be loaded.
  * @param  epnum : endpoint number
  * @retval none
  */
void USB_WriteEmptyTxFifo(unsigned int epnum)
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

  while  ( (USBx_INEP(epnum)->DTXFSTS & USB_OTG_DTXFSTS_INEPTFSAV) > len32b &&
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
    USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;
  }
}

/**
* @brief  USB_CtlError
*         Handle USB low level Error
* @param  pdev: device instance
* @param  req: usb request
* @retval None
*/

void USB_CtlError(void)
{
  USB_StallEP(1, 0);
  USB_StallEP(0, 0);
}

/**
  * @brief  USB_EPSetStall : set a stall condition over an EP
  * @param  num endpoint number
  * @param  is_in endpoint direction
  * @retval none
  */
static void USB_EPSetStall(unsigned int is_in, unsigned int num)
{
  if (is_in)
  {
    if (((USBx_INEP(num)->DIEPCTL) & USB_OTG_DIEPCTL_EPENA) == 0)
    {
      USBx_INEP(num)->DIEPCTL &= ~(USB_OTG_DIEPCTL_EPDIS);
    }
    USBx_INEP(num)->DIEPCTL |= USB_OTG_DIEPCTL_STALL;
  }
  else
  {
    if (((USBx_OUTEP(num)->DOEPCTL) & USB_OTG_DOEPCTL_EPENA) == 0)
    {
      USBx_OUTEP(num)->DOEPCTL &= ~(USB_OTG_DOEPCTL_EPDIS);
    }
    USBx_OUTEP(num)->DOEPCTL |= USB_OTG_DOEPCTL_STALL;
  }
}

void USB_StallEP(unsigned int is_in, unsigned int num)
{
  USB_OTG_EPTypeDef *ep;

  if (is_in)
  {
    ep = &USBHandle.IN_ep[num];
  }
  else
  {
    ep = &USBHandle.OUT_ep[num];
  }

  ep->is_stall = 1;

  USB_EPSetStall(is_in, num);
  if(!num)
  {
    USB_EP0_OutStart();
  }
}

/**
  * @brief  Returns Stall condition.
  * @param  ep_addr: Endpoint Number
  * @retval Stall (1: Yes, 0: No)
  */
unsigned int USB_IsStallEP(unsigned int is_in, unsigned int num)
{
  if(is_in)
  {
    return USBHandle.IN_ep[num].is_stall;
  }
  return USBHandle.OUT_ep[num].is_stall;
}

/**
  * @brief  USB_EPClearStall : Clear a stall condition over an EP
  * @param  num endpoint number
  * @param  is_in endpoint direction
  * @retval none
  */
void USB_EPClearStall(unsigned int is_in, unsigned int num)
{
  unsigned int type;

  if (is_in)
  {
    type = USBHandle.IN_ep[num].type;
    USBx_INEP(num)->DIEPCTL &= ~USB_OTG_DIEPCTL_STALL;
    if (type == USB_EP_TYPE_INTR || type == USB_EP_TYPE_BULK)
    {
      USBx_INEP(num)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM; /* DATA0 */
    }
  }
  else
  {
    type = USBHandle.OUT_ep[num].type;
    USBx_OUTEP(num)->DOEPCTL &= ~USB_OTG_DOEPCTL_STALL;
    if (type == USB_EP_TYPE_INTR || type == USB_EP_TYPE_BULK)
    {
      USBx_OUTEP(num)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM; /* DATA0 */
    }
  }
}

/**
  * @brief  Clear a STALL condition over in an endpoint.
  * @param  ep_addr: endpoint address
  * @retval none
  */
void USB_ClearStallEP(unsigned int is_in, unsigned int num)
{
  USB_OTG_EPTypeDef *ep;

  if (is_in)
  {
    ep = &USBHandle.IN_ep[num];
  }
  else
  {
    ep = &USBHandle.OUT_ep[num];
  }

  ep->is_stall = 0;

  USB_EPClearStall(is_in, num);
}

/**
  * @brief  Send an amount of data.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the transmission buffer
  * @param  len: amount of data to be sent
  * @retval HAL status
  */
void USB_Transmit(unsigned int num, unsigned char *pBuf, unsigned int len)
{
  USB_OTG_EPTypeDef *ep;

  ep = &USBHandle.IN_ep[num];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0;

  if (USBHandle.Cfg->dma_enable)
  {
    ep->dma_addr = (unsigned int)pBuf;
  }

  if (!num)
  {
    USB_EP0StartXfer(1);
  }
  else
  {
    USB_EPStartXfer(1, num);
  }
}

/**
* @brief  USBD_CtlSendStatus
*         send zero length packet on the ctl pipe
* @retval none
*/
void  USB_CtlSendStatus(void)
{
  /* Set EP0 State */
  USBHandle.ep0_state = USB_EP0_STATUS_IN;

  /* Start the transfer */
  USB_Transmit(0, 0, 0);
}

/**
  * @brief  USB_EP0StartXfer : setup and starts a transfer over the EP  0
  * @retval none
  */
void USB_EP0StartXfer(unsigned int is_in)
{
  USB_OTG_EPTypeDef *ep;
  /* IN endpoint */
  if (is_in)
  {
    ep = &USBHandle.IN_ep[0];
    /* Zero Length Packet? */
    if (ep->xfer_len == 0)
    {
      USBx_INEP(0)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);
      USBx_INEP(0)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << 19)) ;
      USBx_INEP(0)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
      USBx_INEP(0)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
      USBx_INEP(0)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);

      if(ep->xfer_len > USB_FS_MAX_PACKET_SIZE)
      {
        ep->xfer_len = USB_FS_MAX_PACKET_SIZE;
      }
      USBx_INEP(0)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << 19)) ;
      USBx_INEP(0)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & ep->xfer_len);

    }

    if (USBHandle.Cfg->dma_enable)
    {
      USBx_INEP(0)->DIEPDMA = (unsigned int)(ep->dma_addr);
    }
    else
    {
      /* Enable the Tx FIFO Empty Interrupt for this EP */
      if (ep->xfer_len > 0)
      {
        USBx_DEVICE->DIEPEMPMSK |= 1;
      }
    }

    /* EP enable, IN data in FIFO */
    USBx_INEP(0)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);
  }
  else /* OUT endpoint */
  {
    ep = &USBHandle.OUT_ep[0];
    /* Program the transfer size and packet count as follows:
    * pktcnt = N
    * xfersize = N * maxpacket
    */
    USBx_OUTEP(0)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_XFRSIZ);
    USBx_OUTEP(0)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_PKTCNT);

    if (ep->xfer_len > 0)
    {
      ep->xfer_len = USB_FS_MAX_PACKET_SIZE;
    }

    USBx_OUTEP(0)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1 << 19));
    USBx_OUTEP(0)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & USB_FS_MAX_PACKET_SIZE);


    if (USBHandle.Cfg->dma_enable)
    {
      USBx_OUTEP(0)->DOEPDMA = (unsigned int)(ep->xfer_buff);
    }

    /* EP enable */
    USBx_OUTEP(0)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
  }
}

/**
  * @brief  USB_EPStartXfer : setup and starts a transfer over an EP
  * @retval none
  */
void USB_EPStartXfer(unsigned int is_in, unsigned int num)
{
  USB_OTG_EPTypeDef *ep;
  unsigned int pktcnt;

  /* IN endpoint */
  if (is_in)
  {
    ep = &USBHandle.IN_ep[num];
    /* Zero Length Packet? */
    if (ep->xfer_len == 0)
    {
      USBx_INEP(num)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);
      USBx_INEP(num)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (1 << 19)) ;
      USBx_INEP(num)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
    }
    else
    {
      /* Program the transfer size and packet count
      * as follows: xfersize = N * maxpacket +
      * short_packet pktcnt = N + (short_packet
      * exist ? 1 : 0)
      */
      USBx_INEP(num)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_XFRSIZ);
      USBx_INEP(num)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_PKTCNT);
      USBx_INEP(num)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_PKTCNT & (((ep->xfer_len + USB_FS_MAX_PACKET_SIZE -1)/ USB_FS_MAX_PACKET_SIZE) << 19)) ;
      USBx_INEP(num)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_XFRSIZ & ep->xfer_len);

      if (ep->type == USB_EP_TYPE_ISOC)
      {
        USBx_INEP(num)->DIEPTSIZ &= ~(USB_OTG_DIEPTSIZ_MULCNT);
        USBx_INEP(num)->DIEPTSIZ |= (USB_OTG_DIEPTSIZ_MULCNT & (1 << 29));
      }
    }

    if (USBHandle.Cfg->dma_enable)
    {
      USBx_INEP(num)->DIEPDMA = ep->dma_addr;
    }
    else
    {
      if (ep->type != USB_EP_TYPE_ISOC)
      {
        /* Enable the Tx FIFO Empty Interrupt for this EP */
        if (ep->xfer_len > 0)
        {
          USBx_DEVICE->DIEPEMPMSK |= 1 << num;
        }
      }
    }

    if (ep->type == USB_EP_TYPE_ISOC)
    {
      if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
      {
        USBx_INEP(num)->DIEPCTL |= USB_OTG_DIEPCTL_SODDFRM;
      }
      else
      {
        USBx_INEP(num)->DIEPCTL |= USB_OTG_DIEPCTL_SD0PID_SEVNFRM;
      }
    }

    /* EP enable, IN data in FIFO */
    USBx_INEP(num)->DIEPCTL |= (USB_OTG_DIEPCTL_CNAK | USB_OTG_DIEPCTL_EPENA);

    if (ep->type == USB_EP_TYPE_ISOC)
    {
      USB_WritePacket(ep->xfer_buff, num, ep->xfer_len);
    }
  }
  else /* OUT endpoint */
  {
    ep = &USBHandle.OUT_ep[num];
    /* Program the transfer size and packet count as follows:
    * pktcnt = N
    * xfersize = N * maxpacket
    */
    USBx_OUTEP(num)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_XFRSIZ);
    USBx_OUTEP(num)->DOEPTSIZ &= ~(USB_OTG_DOEPTSIZ_PKTCNT);

    if (ep->xfer_len == 0)
    {
      USBx_OUTEP(num)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & USB_FS_MAX_PACKET_SIZE);
      USBx_OUTEP(num)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (1 << 19)) ;
    }
    else
    {
      pktcnt = (ep->xfer_len + USB_FS_MAX_PACKET_SIZE -1)/ USB_FS_MAX_PACKET_SIZE;
      USBx_OUTEP(num)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_PKTCNT & (pktcnt << 19));
      USBx_OUTEP(num)->DOEPTSIZ |= (USB_OTG_DOEPTSIZ_XFRSIZ & (USB_FS_MAX_PACKET_SIZE * pktcnt));
    }

    if (USBHandle.Cfg->dma_enable)
    {
      USBx_OUTEP(num)->DOEPDMA = (unsigned int)ep->xfer_buff;
    }

    if (ep->type == USB_EP_TYPE_ISOC)
    {
      if ((USBx_DEVICE->DSTS & ( 1 << 8 )) == 0)
      {
        USBx_OUTEP(num)->DOEPCTL |= USB_OTG_DOEPCTL_SODDFRM;
      }
      else
      {
        USBx_OUTEP(num)->DOEPCTL |= USB_OTG_DOEPCTL_SD0PID_SEVNFRM;
      }
    }
    /* EP enable */
    USBx_OUTEP(num)->DOEPCTL |= (USB_OTG_DOEPCTL_CNAK | USB_OTG_DOEPCTL_EPENA);
  }
}

/**
* @brief  USBD_CtlSendData
*         send data on the ctl pipe
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
void USB_CtlSendData(unsigned char *pbuf, unsigned int len)
{
  /* Set EP0 State */
  USBHandle.ep0_state          = USB_EP0_DATA_IN;
  USBHandle.ep0_total_length = len;
  USBHandle.ep0_rem_length   = len;
  /* Start the transfer */
  USB_Transmit (0x00, pbuf, len);
}

/**
  * @brief  USB_SetDevAddress : Stop the usb device mode
  * @param  address : new device address to be assigned
  *          This parameter can be a value from 0 to 255
  * @retval HAL status
  */
void USB_SetDevAddress(unsigned int address)
{
  USBx_DEVICE->DCFG &= ~ (USB_OTG_DCFG_DAD);
  USBx_DEVICE->DCFG |= (address << 4) & USB_OTG_DCFG_DAD ;
}

/**
* @brief  USB_ClrClassConfig
*         Clear current configuration
* @param  cfgidx: configuration index
* @retval status: USBD_StatusTypeDef
*/
void USB_ClrClassConfig(unsigned int cfgidx)
{
  /* Clear configuration  and De-initialize the Class process*/
  USBHandle.pClass->DeInit(cfgidx);
}

/**
* @brief  USB_SetClassConfig
*        Configure device and start the interface
* @param  cfgidx: configuration index
* @retval status
*/
USB_StatusTypeDef USB_SetClassConfig(unsigned int cfgidx)
{
  if(USBHandle.pClass != NULL)
  {
    /* Set configuration  and Start the Class*/
    if(USBHandle.pClass->Init(cfgidx) == 0)
    {
      return USB_OK;
    }
  }
  return USB_FAIL;
}

/**
  * @brief  Receive an amount of data.
  * @param  ep_addr: endpoint address
  * @param  pBuf: pointer to the reception buffer
  * @param  len: amount of data to be received
  * @retval HAL status
  */
void USB_EP_Receive(unsigned int num, unsigned char *pBuf, unsigned int len)
{
  USB_OTG_EPTypeDef *ep;

  ep = &USBHandle.OUT_ep[num];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0;

  if (USBHandle.Cfg->dma_enable)
  {
    ep->dma_addr = (unsigned int)pBuf;
  }

  if (!num)
  {
    USB_EP0StartXfer(0);
  }
  else
  {
    USB_EPStartXfer(0, num);
  }
}

/**
* @brief  USB_RunTestMode
*         Launch test mode process
* @retval none
*/
void USB_RunTestMode(void)
{
}

/**
* @brief  USB_CtlReceiveStatus
*         receive zero length packet on the ctl pipe
* @retval none
*/
void USB_CtlReceiveStatus(void)
{
  /* Set EP0 State */
  USBHandle.ep0_state = USB_EP0_STATUS_OUT;

  /* Start the transfer */
  USB_EP_Receive (0, NULL, 0);
}

/**
  * @brief  USBD_GetLen
  *         return the string length
   * @param  buf : pointer to the ascii string buffer
  * @retval string length
  */
static unsigned int USB_GetLen(unsigned char *buf)
{
  unsigned int len = 0;

  while (*buf++)
  {
    len++;
  }

  return len;
}

/**
  * @brief  USB_GetString
  *         Convert Ascii string into unicode one
  * @param  desc : descriptor buffer
  * @param  unicode : Formatted string buffer (unicode)
  * @param  len : descriptor length
  * @retval None
  */
void USB_GetString(unsigned char *desc, unsigned char *unicode, unsigned int *len)
{
  unsigned int idx = 0;

  if (desc != NULL)
  {
    unicode[idx++] = *len =  USB_GetLen(desc) * 2 + 2;
    unicode[idx++] =  USB_DESC_TYPE_STRING;

    while (*desc != '\0')
    {
      unicode[idx++] = *desc++;
      unicode[idx++] =  0x00;
    }
  }
}

/**
  * @brief  De-activate and de-initialize an endpoint
  * @retval none
  */
void USB_DeactivateEndpoint(unsigned int is_in, unsigned int num)
{
  /* Read DEPCTLn register */
  if (is_in)
  {
    USBx_DEVICE->DEACHMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1 << num)));
    USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_IEPM & ((1 << num)));
    USBx_INEP(num)->DIEPCTL &= ~ USB_OTG_DIEPCTL_USBAEP;
  }
  else
  {
    USBx_DEVICE->DEACHMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1 << num) << 16));
    USBx_DEVICE->DAINTMSK &= ~(USB_OTG_DAINTMSK_OEPM & ((1 << num) << 16));
    USBx_OUTEP(num)->DOEPCTL &= ~USB_OTG_DOEPCTL_USBAEP;
  }
}

/**
* @brief  USB_CtlPrepareRx
*         receive data on the ctl pipe
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
void USB_CtlPrepareRx (unsigned char *pbuf, unsigned int len)
{
  /* Set EP0 State */
  USBHandle.ep0_state = USB_EP0_DATA_OUT;
  USBHandle.ep0_total_length = len;
  USBHandle.ep0_rem_length   = len;
  /* Start the transfer */
  USB_EP_Receive (0,
                  pbuf,
                  len);
}

/**
  * @brief  Get Received Data Size.
  * @param  hpcd: PCD handle
  * @param  ep_addr: endpoint address
  * @retval Data Size
  */
unsigned int USB_EP_GetRxCount(unsigned int num)
{
  return USBHandle.OUT_ep[num].xfer_count;
}

/**
  * @brief  USB_ReadDevAllInEpInterrupt: return the USB device IN endpoints interrupt status
  * @retval status
  */
unsigned int USB_ReadDevAllInEpInterrupt (void)
{
  unsigned int v;
  v  = USBx_DEVICE->DAINT;
  v &= USBx_DEVICE->DAINTMSK;
  return v & 0xFFFF;
}
