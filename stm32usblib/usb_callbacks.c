#include <usb.h>

const USB_Callbacks Default_Callbacks = {
  USB_Default_ResetCallback,
  USB_Default_ConnectCallback,
  USB_Default_DisconnectCallback,
  USB_Default_SuspendCallback,
  USB_Default_ResumeCallback,
  USB_Default_SOFCallback,
  USB_Default_SetupStageCallback,
  USB_Default_ISOOUTIncompleteCallback,
  USB_Default_ISOINIncompleteCallback,
  USB_Default_DataOutStageCallback,
  USB_Default_DataInStageCallback,
  USB_Default_LPMCallback
};

void USB_Default_ResetCallback(void)
{
  /* Open EP0 OUT */
  USB_EP_Open(0, 0,
              USB_EP_TYPE_CTRL);

  /* Open EP0 IN */
  USB_EP_Open(1, 0,
              USB_EP_TYPE_CTRL);

  USBHandle.dev_state = USB_STATE_DEFAULT;

  USBHandle.pClass->DeInit(USBHandle.dev_config);
}

void USB_Default_ConnectCallback(void)
{
}

void USB_Default_DisconnectCallback(void)
{
  USBHandle.dev_state = USB_STATE_DEFAULT;
  USBHandle.pClass->DeInit(USBHandle.dev_config);
}

void USB_Default_SuspendCallback(void)
{
  USBHandle.dev_old_state =  USBHandle.dev_state;
  USBHandle.dev_state  = USB_STATE_SUSPENDED;
}

void USB_Default_ResumeCallback(void)
{
  USBHandle.dev_state = USBHandle.dev_old_state;
}

void USB_Default_SOFCallback(void)
{
  if (USBHandle.dev_state == USB_STATE_CONFIGURED)
  {
    if(USBHandle.pClass->SOF != NULL)
    {
      USBHandle.pClass->SOF();
    }
  }
}

void USB_Default_SetupStageCallback(void)
{
  USBHandle.ep0_state = USB_EP0_SETUP;
  USBHandle.ep0_data_len = USBHandle.Setup.req.wLength;

  switch (USBHandle.Setup.req.bmRequest & 0x1F)
  {
    case USB_REQ_RECIPIENT_DEVICE:
      USB_StdDevReq();
      break;

    case USB_REQ_RECIPIENT_INTERFACE:
      USB_StdItfReq();
      break;

    case USB_REQ_RECIPIENT_ENDPOINT:
      USB_StdEPReq();
      break;

    default:
      USB_StallEP(USBHandle.Setup.req.bmRequest & USB_EP_IN, USBHandle.Setup.req.bmRequest & 0x7F);
      break;
  }
}

void USB_Default_ISOOUTIncompleteCallback(void)
{
}

void USB_Default_ISOINIncompleteCallback(void)
{
}

void USB_Default_DataOutStageCallback(unsigned int epnum)
{
  if (epnum == 0)
  {
    if (USBHandle.ep0_state == USB_EP0_DATA_OUT)
    {
      if(USBHandle.ep0_rem_length > USB_FS_MAX_PACKET_SIZE)
      {
        USBHandle.ep0_rem_length -=  USB_FS_MAX_PACKET_SIZE;

        USB_EP_Receive (0, USBHandle.OUT_ep[0].xfer_buff, MIN(USBHandle.ep0_rem_length ,USB_FS_MAX_PACKET_SIZE));
      }
      else
      {
        if((USBHandle.pClass->EP0_RxReady != NULL)&&
           (USBHandle.dev_state == USB_STATE_CONFIGURED))
        {
          USBHandle.pClass->EP0_RxReady();
        }
        USB_CtlSendStatus();
      }
    }
  }
  else if((USBHandle.pClass->DataOut != NULL)&&
          (USBHandle.dev_state == USB_STATE_CONFIGURED))
  {
    USBHandle.pClass->DataOut(epnum);
  }
}

void USB_Default_DataInStageCallback(unsigned int epnum)
{
  if(epnum == 0)
  {
    if (USBHandle.ep0_state == USB_EP0_DATA_IN)
    {
      if(USBHandle.ep0_rem_length > USB_FS_MAX_PACKET_SIZE)
      {
        USBHandle.ep0_rem_length -= USB_FS_MAX_PACKET_SIZE;

        USB_Transmit (0, USBHandle.IN_ep[0].xfer_buff, USBHandle.ep0_rem_length);

        /* Prepare endpoint for premature end of transfer */
        USB_EP_Receive (0,
                            NULL,
                            0);
      }
      else
      { /* last packet is MPS multiple, so send ZLP packet */
        if((USBHandle.ep0_total_length % USB_FS_MAX_PACKET_SIZE == 0) &&
           (USBHandle.ep0_total_length >= USB_FS_MAX_PACKET_SIZE) &&
           (USBHandle.ep0_total_length < USBHandle.ep0_data_len))
        {

          USB_Transmit(0, NULL, 0);
          USBHandle.ep0_data_len = 0;

          /* Prepare endpoint for premature end of transfer */
          USB_EP_Receive (0,
                          NULL,
                          0);
        }
        else
        {
          if((USBHandle.pClass->EP0_TxSent != NULL)&&
             (USBHandle.dev_state == USB_STATE_CONFIGURED))
          {
            USBHandle.pClass->EP0_TxSent();
          }
          USB_CtlReceiveStatus();
        }
      }
    }
    if (USBHandle.dev_test_mode)
    {
      USB_RunTestMode();
      USBHandle.dev_test_mode = 0;
    }
  }
  else if((USBHandle.pClass->DataIn != NULL)&&
          (USBHandle.dev_state == USB_STATE_CONFIGURED))
  {
    USBHandle.pClass->DataIn(epnum);
  }
}

void USB_Default_LPMCallback(void)
{
}
