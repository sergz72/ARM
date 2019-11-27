#include <usb.h>

#define STS_GOUT_NAK                           1U
#define STS_DATA_UPDT                          2U
#define STS_XFER_COMP                          3U
#define STS_SETUP_COMP                         4U
#define STS_SETUP_UPDT                         6U

#define CLEAR_IN_EP_INTR(__EPNUM__, __INTERRUPT__)          (USBx_INEP(__EPNUM__)->DIEPINT = (__INTERRUPT__))
#define CLEAR_OUT_EP_INTR(__EPNUM__, __INTERRUPT__)         (USBx_OUTEP(__EPNUM__)->DOEPINT = (__INTERRUPT__))

#define USB_MASK_INTERRUPT(__INTERRUPT__)     (USBHandle.Instance->GINTMSK &= ~(__INTERRUPT__))
#define USB_UNMASK_INTERRUPT(__INTERRUPT__)   (USBHandle.Instance->GINTMSK |= (__INTERRUPT__))

static void OEPINTHandler(void);
static void IEPINTHandler(void);
static void USBRSTHandler(void);

void USBInterruptHandler(void)
{
	unsigned int status = USB_ReadInterrupts();
	unsigned int temp;
  USB_OTG_EPTypeDef *ep;

  /* avoid spurious interrupt */
	if (status == 0)
		return;
	if (status & USB_OTG_GINTSTS_MMIS)
	{
    /* incorrect mode, acknowledge the interrupt */
    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_MMIS;
	}
	
	if (status & USB_OTG_GINTSTS_OEPINT)
	{
	  OEPINTHandler();
	}
	
	if (status & USB_OTG_GINTSTS_IEPINT)
	{
	  IEPINTHandler();
	}
	
  /* Handle Resume Interrupt */
	if (status & USB_OTG_GINTSTS_WKUINT)
	{
		/* Clear the Remote Wake-up Signaling */
		USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;

    USBHandle.Callbacks->Resume_Callback();

    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_WKUINT;
	}

  /* Handle Suspend Interrupt */
	if (status & USB_OTG_GINTSTS_USBSUSP)
	{
		if((USBx_DEVICE->DSTS & USB_OTG_DSTS_SUSPSTS) == USB_OTG_DSTS_SUSPSTS)
		{
      USBHandle.Callbacks->Suspend_Callback();
		}
    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_USBSUSP;
	}

#ifndef STM32F2
  /* Handle LPM Interrupt */ 
	if (status & USB_OTG_GINTSTS_LPMINT)
	{
    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_LPMINT;

    USBHandle.Callbacks->LPM_Callback();
	}
#endif

  /* Handle Reset Interrupt */
	if (status & USB_OTG_GINTSTS_USBRST)
	{
	  USBRSTHandler();
	}

  /* Handle Enumeration done Interrupt */
	if (status & USB_OTG_GINTSTS_ENUMDNE)
	{
		USB_ActivateSetup();
    USBHandle.Instance->GUSBCFG &= ~USB_OTG_GUSBCFG_TRDT;

    USBHandle.Instance->GUSBCFG |= (unsigned int)((USB_TRDT_TIME << 10) & USB_OTG_GUSBCFG_TRDT);

    USBHandle.Callbacks->Reset_Callback();

    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_ENUMDNE;
	}

  /* Handle RxQLevel Interrupt */
	if (status & USB_OTG_GINTSTS_RXFLVL)
	{
		USB_MASK_INTERRUPT(USB_OTG_GINTSTS_RXFLVL);
    temp = USBHandle.Instance->GRXSTSP;
    ep = &USBHandle.OUT_ep[temp & USB_OTG_GRXSTSP_EPNUM];

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

    USB_UNMASK_INTERRUPT(USB_OTG_GINTSTS_RXFLVL);
	}

	/* Handle SOF Interrupt */
	if(status & USB_OTG_GINTSTS_SOF)
	{
    USBHandle.Callbacks->SOF_Callback();
    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_SOF;
	}
	
	/* Handle Incomplete ISO IN Interrupt */
	if(status & USB_OTG_GINTSTS_IISOIXFR)
	{
	  //TODO: epnum?
    USBHandle.Callbacks->ISOINIncomplete_Callback();
    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_IISOIXFR;
	} 
	
	/* Handle Incomplete ISO OUT Interrupt */
	if(status & USB_OTG_GINTSTS_PXFR_INCOMPISOOUT)
	{
    //TODO: epnum?
    USBHandle.Callbacks->ISOOUTIncomplete_Callback();
    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_PXFR_INCOMPISOOUT;
	} 
	
	/* Handle Connection event Interrupt */
	if(status & USB_OTG_GINTSTS_SRQINT)
	{
    USBHandle.Callbacks->Connect_Callback();
    USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_SRQINT;
	} 
	
	/* Handle Disconnection event Interrupt */
	if(status & USB_OTG_GINTSTS_OTGINT)
	{
		temp = USBHandle.Instance->GOTGINT;
		
		if(temp & USB_OTG_GOTGINT_SEDET)
		{
      USBHandle.Callbacks->Disconnect_Callback();
		}
    USBHandle.Instance->GOTGINT |= temp;
	}
}

static void OEPINTHandler(void)
{
  unsigned int epnum, ep_intr, epint;

  epnum = 0;

  /* Read in the device interrupt bits */
  ep_intr = USB_ReadDevAllOutEpInterrupt();

  while (ep_intr)
  {
    if (ep_intr & 0x1)
    {
      epint = USB_ReadDevOutEPInterrupt(epnum);

      if(( epint & USB_OTG_DOEPINT_XFRC) == USB_OTG_DOEPINT_XFRC)
      {
        CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_XFRC);

        if(USBHandle.Cfg->dma_enable)
        {
          USBHandle.OUT_ep[epnum].xfer_count = USB_FS_MAX_PACKET_SIZE - (USBx_OUTEP(epnum)->DOEPTSIZ & USB_OTG_DOEPTSIZ_XFRSIZ);
          USBHandle.OUT_ep[epnum].xfer_buff += USB_FS_MAX_PACKET_SIZE;
        }

        USBHandle.Callbacks->DataOutStage_Callback(epnum);
        if(USBHandle.Cfg->dma_enable)
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
        CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_STUP);
      }

      if(( epint & USB_OTG_DOEPINT_OTEPDIS) == USB_OTG_DOEPINT_OTEPDIS)
      {
        CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPDIS);
      }
      // Clear Status Phase Received interrupt
      if(( epint & USB_OTG_DOEPINT_OTEPSPR) == USB_OTG_DOEPINT_OTEPSPR)
      {
        CLEAR_OUT_EP_INTR(epnum, USB_OTG_DOEPINT_OTEPSPR);
      }
    }
    epnum++;
    ep_intr >>= 1;
  }
}

static void IEPINTHandler(void)
{
  unsigned int epnum, ep_intr, epint, fifoemptymsk;

  epnum = 0;

  /* Read in the device interrupt bits */
  ep_intr = USB_ReadDevAllInEpInterrupt();

  while ( ep_intr )
  {
    if (ep_intr & 0x1) /* In ITR */
    {
      epint = USB_ReadDevInEPInterrupt(epnum);

      if(( epint & USB_OTG_DIEPINT_XFRC) == USB_OTG_DIEPINT_XFRC)
      {
        fifoemptymsk = 0x1 << epnum;
        USBx_DEVICE->DIEPEMPMSK &= ~fifoemptymsk;

        CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_XFRC);

        if (USBHandle.Cfg->dma_enable)
        {
          USBHandle.IN_ep[epnum].xfer_buff += USB_FS_MAX_PACKET_SIZE;
        }

        USBHandle.Callbacks->DataInStage_Callback(epnum);

        if (USBHandle.Cfg->dma_enable)
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
        CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_TOC);
      }
      if(( epint & USB_OTG_DIEPINT_ITTXFE) == USB_OTG_DIEPINT_ITTXFE)
      {
        CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_ITTXFE);
      }
      if(( epint & USB_OTG_DIEPINT_INEPNE) == USB_OTG_DIEPINT_INEPNE)
      {
        CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_INEPNE);
      }
      if(( epint & USB_OTG_DIEPINT_EPDISD) == USB_OTG_DIEPINT_EPDISD)
      {
        CLEAR_IN_EP_INTR(epnum, USB_OTG_DIEPINT_EPDISD);
      }
      if(( epint & USB_OTG_DIEPINT_TXFE) == USB_OTG_DIEPINT_TXFE)
      {
        USB_WriteEmptyTxFifo(epnum);
      }
    }
    epnum++;
    ep_intr >>= 1;
  }
}

static void USBRSTHandler(void)
{
  unsigned int i;

  USBx_DEVICE->DCTL &= ~USB_OTG_DCTL_RWUSIG;
  USB_FlushTxFifo(0);

  for (i = 0; i < USBHandle.Cfg->dev_endpoints ; i++)
  {
    USBx_INEP(i)->DIEPINT = 0xFF;
    USBx_OUTEP(i)->DOEPINT = 0xFF;
  }
  USBx_DEVICE->DAINT = 0xFFFFFFFF;
  USBx_DEVICE->DAINTMSK |= 0x10001;

  if(USBHandle.Cfg->use_dedicated_ep1)
  {
    USBx_DEVICE->DOUTEP1MSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM);
    USBx_DEVICE->DINEP1MSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
  }
  else
  {
    USBx_DEVICE->DOEPMSK |= (USB_OTG_DOEPMSK_STUPM | USB_OTG_DOEPMSK_XFRCM | USB_OTG_DOEPMSK_EPDM | USB_OTG_DOEPMSK_OTEPSPRM);
    USBx_DEVICE->DIEPMSK |= (USB_OTG_DIEPMSK_TOM | USB_OTG_DIEPMSK_XFRCM | USB_OTG_DIEPMSK_EPDM);
  }

  /* Set Default Address to 0 */
  USBx_DEVICE->DCFG &= ~USB_OTG_DCFG_DAD;

  /* setup EP0 to receive SETUP packets */
	USB_EP0_OutStart();

  USBHandle.Instance->GINTSTS = USB_OTG_GINTSTS_USBRST;
}
