#include <usb_cdc.h>

static unsigned int USB_CDC_Init (unsigned int cfgidx);
static void  USB_CDC_DeInit (unsigned int cfgidx);
static void  USB_CDC_Setup (unsigned int);
static void  USB_CDC_DataIn (unsigned int epnum);
static void  USB_CDC_DataOut (unsigned int epnum);
static void USB_CDC_EP0_RxReady (void);
static unsigned int USB_CDC_ReceivePacket(unsigned int epnum);
static unsigned int USB_CDC_TransmitPacket(unsigned int epnum);

static void USB_CDC_SetTxBuffer(unsigned int epnum, unsigned char *pbuff, unsigned int length);

/* CDC interface class callbacks structure */
const USB_ClassTypeDef CDC_Class =
  {
    USB_CDC_Init,
    USB_CDC_DeInit,
    USB_CDC_Setup,
    NULL,                 /* EP0_TxSent, */
    USB_CDC_EP0_RxReady,
    USB_CDC_DataIn,
    USB_CDC_DataOut,
    NULL,
    NULL,
    NULL
  };

static void CDC_Init(USB_CDC_HandleTypeDef *hcdc)
{
}

static void CDC_DeInit(USB_CDC_HandleTypeDef *hcdc)
{
}

static void CDC_Control(USB_CDC_HandleTypeDef *hcdc, unsigned int cmd, unsigned char* pbuf, unsigned int length)
{
}

static void CDC_Receive(USB_CDC_HandleTypeDef *hcdc)
{
  while (USB_CDC_ReceivePacket(hcdc->data_endpoint_no) != USB_OK)
    ;
  hcdc->serial_receive(hcdc->RxBuffer, hcdc->RxLength);
}

unsigned int CDC_Transmit(unsigned int epnum, unsigned char* Buf, unsigned int Len)
{
  unsigned int result;
  //USB_DEBUG1("CDC_Transmit", epnum);
  USB_CDC_HandleTypeDef *hcdc = (USB_CDC_HandleTypeDef*)USBHandle.Cfg->pEpClassData[epnum];
  if (hcdc->TxState != 0)
  {
    return USB_BUSY;
  }
  USB_CDC_SetTxBuffer(epnum, Buf, Len);
  result = USB_CDC_TransmitPacket(epnum);
  return result;
}

/**
  * @brief  USBD_CDC_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static unsigned int USB_CDC_Init (unsigned int cfgidx)
{
  unsigned int i;
  USB_CDC_HandleTypeDef *hcdc;

  for (i = 0; i < USB_EP_MAX; i++)
  {
    hcdc = USBHandle.Cfg->pCDCClassData[i];
    if (!hcdc)
      return 0;
    if (hcdc->is_initialized)
      continue;

    /* Open EP IN */
    USB_EP_Open(1, hcdc->data_endpoint_no,
                USB_EP_TYPE_BULK);

    /* Open EP OUT */
    USB_EP_Open(0, hcdc->data_endpoint_no,
                USB_EP_TYPE_BULK);

    /* Open Command IN EP */
    USB_EP_Open(1, hcdc->control_endpoint_no,
                USB_EP_TYPE_INTR);

    /* Init  physical Interface components */
    CDC_Init(hcdc);

    /* Init Xfer states */
    hcdc->TxState = 0;
    hcdc->RxState = 0;
    hcdc->TxBuffer = NULL;
    hcdc->CmdOpCode = 0xFF;
    hcdc->is_initialized = 1;

    /* Prepare Out endpoint to receive next packet */
    USB_EP_Receive(hcdc->data_endpoint_no,
                   hcdc->RxBuffer,
                   CDC_DATA_FS_OUT_PACKET_SIZE);
  }

  return 0;
}

/**
  * @brief  USBD_CDC_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static void  USB_CDC_DeInit (unsigned int cfgidx)
{
  unsigned int i;
  USB_CDC_HandleTypeDef   *hcdc;

  for (i = 0; i < USB_EP_MAX; i++)
  {
    hcdc = USBHandle.Cfg->pCDCClassData[i];
    if (!hcdc)
      return;
    if (!hcdc->is_initialized)
      continue;

    USB_DeactivateEndpoint(1, hcdc->data_endpoint_no);

    USB_DeactivateEndpoint(0, hcdc->data_endpoint_no);

    USB_DeactivateEndpoint(1, hcdc->control_endpoint_no);

    hcdc->TxBuffer = NULL;
    hcdc->is_initialized = 0;

    /* DeInit  physical Interface components */
    CDC_DeInit(hcdc);
  }
}

/**
  * @brief  USBD_CDC_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static void  USB_CDC_Setup (unsigned int interface_no)
{
  USB_CDC_HandleTypeDef   *hcdc = (USB_CDC_HandleTypeDef*) USBHandle.Cfg->pItfClassData[interface_no];
  static unsigned char ifalt = 0;

  switch (USBHandle.Setup.req.bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS :
      if (USBHandle.Setup.req.wLength)
      {
        if (USBHandle.Setup.req.bmRequest & USB_EP_IN)
        {
          CDC_Control(hcdc, USBHandle.Setup.req.bRequest,
                      (unsigned char*)hcdc->data,
                      USBHandle.Setup.req.wLength);
          USB_CtlSendData ((unsigned char*)hcdc->data,
                           USBHandle.Setup.req.wLength);
        }
        else
        {
          hcdc->CmdOpCode = USBHandle.Setup.req.bRequest;
          hcdc->CmdLength = USBHandle.Setup.req.wLength;

          USB_CtlPrepareRx ((unsigned char*)hcdc->data,
                            USBHandle.Setup.req.wLength);
        }
      }
      else
      {
        CDC_Control(hcdc, USBHandle.Setup.req.bRequest, USBHandle.Setup.data, 0);
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (USBHandle.Setup.req.bRequest)
      {
        case USB_REQ_GET_INTERFACE :
          USB_CtlSendData (&ifalt, 1);
          break;

        case USB_REQ_SET_INTERFACE :
          break;
      }

    default:
      break;
  }
}

/**
  * @brief  USBD_CDC_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static void USB_CDC_DataIn (unsigned int epnum)
{
  USB_CDC_HandleTypeDef *hcdc = (USB_CDC_HandleTypeDef*) USBHandle.Cfg->pEpClassData[epnum];

  hcdc->TxState = 0;
}

/**
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static void USB_CDC_DataOut (unsigned int epnum)
{
  USB_CDC_HandleTypeDef *hcdc = (USB_CDC_HandleTypeDef*) USBHandle.Cfg->pEpClassData[epnum];

  /* Get the received data length */
  hcdc->RxLength = USB_EP_GetRxCount(epnum);

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application Xfer */
  CDC_Receive(hcdc);
}



/**
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static void USB_CDC_EP0_RxReady()
{
  unsigned int i;
  USB_CDC_HandleTypeDef *hcdc;

  for (i = 0; i < USB_EP_MAX; i++)
  {
    hcdc = USBHandle.Cfg->pCDCClassData[i];
    if (!hcdc)
      return;

    if (hcdc->CmdOpCode != 0xFF)
    {
      CDC_Control(hcdc, hcdc->CmdOpCode, (unsigned char *) hcdc->data, hcdc->CmdLength);
      hcdc->CmdOpCode = 0xFF;
    }
  }
}

/**
  * @brief  USBD_CDC_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
static void USB_CDC_SetTxBuffer(unsigned int epnum, unsigned char *pbuff, unsigned int length)
{
  USB_CDC_HandleTypeDef   *hcdc = (USB_CDC_HandleTypeDef*) USBHandle.Cfg->pEpClassData[epnum];

  hcdc->TxBuffer = pbuff;
  hcdc->TxLength = length;
}

/**
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static unsigned int USB_CDC_TransmitPacket(unsigned int epnum)
{
  USB_CDC_HandleTypeDef   *hcdc = (USB_CDC_HandleTypeDef*) USBHandle.Cfg->pEpClassData[epnum];

  if(hcdc->TxBuffer != NULL)
  {
    if(hcdc->TxState == 0)
    {
      /* Tx Transfer in progress */
      hcdc->TxState = 1;

      /* Transmit next packet */
      USB_Transmit(hcdc->data_endpoint_no,
                   hcdc->TxBuffer,
                   hcdc->TxLength);

      return USB_OK;
    }
    else
    {
      return USB_BUSY;
    }
  }
  else
  {
    return USB_FAIL;
  }
}


/**
  * @brief  USBD_CDC_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
static unsigned int USB_CDC_ReceivePacket(unsigned int epnum)
{
  USB_CDC_HandleTypeDef   *hcdc = (USB_CDC_HandleTypeDef*) USBHandle.Cfg->pEpClassData[epnum];

  /* Prepare Out endpoint to receive next packet */
  USB_EP_Receive(hcdc->data_endpoint_no,
                 hcdc->RxBuffer,
                 CDC_DATA_FS_OUT_PACKET_SIZE);
  return USB_OK;
}
