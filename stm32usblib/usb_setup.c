#include <usb.h>

static void USB_GetDescriptor(void);
static void USB_SetAddress(void);
static void USB_SetConfig(void);
static void USB_GetConfig(void);
static void USB_GetStatus(void);
static void USB_ClrFeature(void);
static void USB_SetFeature(void);

/**
* @brief  USB_StdDevReq
*         Handle standard usb device requests
* @retval none
*/
void USB_StdDevReq(void)
{
  switch (USBHandle.Setup.req.bRequest)
  {
    case USB_REQ_GET_DESCRIPTOR:
      USB_GetDescriptor() ;
      break;

    case USB_REQ_SET_ADDRESS:
      USB_SetAddress();
      break;

    case USB_REQ_SET_CONFIGURATION:
      USB_SetConfig();
      break;

    case USB_REQ_GET_CONFIGURATION:
      USB_GetConfig();
      break;

    case USB_REQ_GET_STATUS:
      USB_GetStatus();
      break;

    case USB_REQ_SET_FEATURE:
      USB_SetFeature();
      break;

    case USB_REQ_CLEAR_FEATURE:
      USB_ClrFeature();
      break;

    default:
      USB_CtlError();
      break;
  }
}

/**
* @brief  USB_StdItfReq
*         Handle standard usb interface requests
* @retval none
*/
void USB_StdItfReq(void)
{
  unsigned int interface_no;

  switch (USBHandle.dev_state)
  {
    case USB_STATE_CONFIGURED:
      interface_no = LOBYTE(USBHandle.Setup.req.wIndex);
      if (interface_no <= USB_MAX_NUM_INTERFACES)
      {
        //USB_DEBUG1("USB_StdItfReq", interface_no);
        USBHandle.pClass->Setup(interface_no);

        if(USBHandle.Setup.req.wLength == 0)
        {
          USB_CtlSendStatus();
        }
      }
      else
      {
        USB_CtlError();
      }
      break;

    default:
      USB_CtlError();
      break;
  }
}

/**
* @brief  USB_StdEPReq
*         Handle standard usb endpoint requests
* @retval none
*/
void USB_StdEPReq(void)
{
  unsigned int ep_addr = LOBYTE(USBHandle.Setup.req.wIndex);
  unsigned short status;
  unsigned int num, is_in;


  /* Check if it is a class request */
  if ((USBHandle.Setup.req.bmRequest & 0x60) == 0x20)
  {
    //USB_DEBUG("USB_StdEPReq1");
    //USBHandle.pClass->Setup();
    return;
  }

  is_in = ep_addr & USB_EP_IN;
  num = ep_addr & 0x7F;

  switch (USBHandle.Setup.req.bRequest)
  {

    case USB_REQ_SET_FEATURE :

      switch (USBHandle.dev_state)
      {
        case USB_STATE_ADDRESSED:
          if (num)
          {
            USB_StallEP(is_in, num);
          }
          break;

        case USB_STATE_CONFIGURED:
          if (USBHandle.Setup.req.wValue == USB_FEATURE_EP_HALT)
          {
            if (num)
            {
              USB_StallEP(is_in, num);
            }
          }
          //USB_DEBUG2("USB_StdEPReq2", is_in, num);
          //USBHandle.pClass->Setup();
          USB_CtlSendStatus();

          break;

        default:
          USB_CtlError();
          break;
      }
      break;

    case USB_REQ_CLEAR_FEATURE :

      switch (USBHandle.dev_state)
      {
        case USB_STATE_ADDRESSED:
          if (num)
          {
            USB_StallEP(is_in, num);
          }
          break;

        case USB_STATE_CONFIGURED:
          if (USBHandle.Setup.req.wValue == USB_FEATURE_EP_HALT)
          {
            if (num)
            {
              USB_ClearStallEP(is_in, num);
              //USB_DEBUG2("USB_StdEPReq3", is_in, num);
              //USBHandle.pClass->Setup();
            }
            USB_CtlSendStatus();
          }
          break;

        default:
          USB_CtlError();
          break;
      }
      break;

    case USB_REQ_GET_STATUS:
      switch (USBHandle.dev_state)
      {
        case USB_STATE_ADDRESSED:
          if (num)
          {
            USB_StallEP(is_in, num);
          }
          break;

        case USB_STATE_CONFIGURED:
          if (USB_IsStallEP(is_in, num))
          {
            status = 0x0001;
          }
          else
          {
            status = 0x0000;
          }

          USB_CtlSendData ((unsigned char*)&status, 2);
          break;

        default:
          USB_CtlError();
          break;
      }
      break;

    default:
      break;
  }
}

static unsigned char *USB_GetFSConfigDescriptor(unsigned int *length)
{
  *length = USB_DESC_SIZ;
  return (unsigned char*)USB_CfgFSDesc;
}

/**
* @brief  USBD_GetDescriptor
*         Handle Get Descriptor requests
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_GetDescriptor(void)
{
  unsigned int len;
  unsigned char *pbuf;

  switch (USBHandle.Setup.req.wValue >> 8)
  {
#ifdef USBD_LPM_ENABLED
    case USB_DESC_TYPE_BOS:
    pbuf = USBHandle.pDesc->GetBOSDescriptor(&len);
    break;
#endif
    case USB_DESC_TYPE_DEVICE:
      pbuf = USBHandle.pDesc->GetDeviceDescriptor(&len);
      break;

    case USB_DESC_TYPE_CONFIGURATION:
      pbuf = USB_GetFSConfigDescriptor(&len);
      break;

    case USB_DESC_TYPE_STRING:
      switch (LOBYTE(USBHandle.Setup.req.wValue))
      {
        case USB_IDX_LANGID_STR:
          pbuf = USBHandle.pDesc->GetLangIDStrDescriptor(&len);
          break;

        case USB_IDX_MFC_STR:
          pbuf = USBHandle.pDesc->GetManufacturerStrDescriptor(&len);
          break;

        case USB_IDX_PRODUCT_STR:
          pbuf = USBHandle.pDesc->GetProductStrDescriptor(&len);
          break;

        case USB_IDX_SERIAL_STR:
          pbuf = USBHandle.pDesc->GetSerialStrDescriptor(&len);
          break;

        case USB_IDX_CONFIG_STR:
          pbuf = USBHandle.pDesc->GetConfigurationStrDescriptor(&len);
          break;

        case USB_IDX_INTERFACE_STR:
          pbuf = USBHandle.pDesc->GetInterfaceStrDescriptor(&len);
          break;

        default:
#ifdef USBD_SUPPORT_USER_STRING
          pbuf = pdev->pClass->GetUsrStrDescriptor(pdev, req->wValue, &len);
      break;
#else
          USB_CtlError();
          return;
#endif
      }
      break;
    case USB_DESC_TYPE_DEVICE_QUALIFIER:
      USB_CtlError();
      return;

    case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
      USB_CtlError();
      return;

    default:
      USB_CtlError();
      return;
  }

  if(len && USBHandle.Setup.req.wLength)
  {
    len = MIN(len , USBHandle.Setup.req.wLength);

    USB_CtlSendData (pbuf, len);
  }
}

/**
* @brief  USB_SetAddress
*         Set device address
* @retval none
*/
static void USB_SetAddress(void)
{
  unsigned int dev_addr;

  if (!USBHandle.Setup.req.wIndex && !USBHandle.Setup.req.wLength)
  {
    if (USBHandle.dev_state == USB_STATE_CONFIGURED)
    {
      USB_CtlError();
    }
    else
    {
      dev_addr = USBHandle.Setup.req.wValue & 0x7F;
      USBHandle.dev_address = dev_addr;
      USB_SetDevAddress(dev_addr);
      USB_CtlSendStatus();

      if (dev_addr)
      {
        USBHandle.dev_state  = USB_STATE_ADDRESSED;
      }
      else
      {
        USBHandle.dev_state  = USB_STATE_DEFAULT;
      }
    }
  }
  else
  {
    USB_CtlError();
  }
}

/**
* @brief  USBD_SetConfig
*         Handle Set device configuration request
* @retval status
*/
static void USB_SetConfig(void)
{

  unsigned int cfgidx = LOBYTE(USBHandle.Setup.req.wValue);

  if (cfgidx > USB_MAX_NUM_CONFIGURATION)
  {
    USB_CtlError();
  }
  else
  {
    switch (USBHandle.dev_state)
    {
      case USB_STATE_ADDRESSED:
        if (cfgidx)
        {
          USBHandle.dev_config = cfgidx;
          USBHandle.dev_state = USB_STATE_CONFIGURED;
          if(USB_SetClassConfig(cfgidx) == USB_FAIL)
          {
            USB_CtlError();
            return;
          }
          USB_CtlSendStatus();
        }
        else
        {
          USB_CtlSendStatus();
        }
        break;

      case USB_STATE_CONFIGURED:
        if (!cfgidx)
        {
          USBHandle.dev_state = USB_STATE_ADDRESSED;
          USBHandle.dev_config = cfgidx;
          USB_ClrClassConfig(cfgidx);
          USB_CtlSendStatus();

        }
        else  if (cfgidx != USBHandle.dev_config)
        {
          /* Clear old configuration */
          USB_ClrClassConfig(USBHandle.dev_config);

          /* set new configuration */
          USBHandle.dev_config = cfgidx;
          if(USB_SetClassConfig(cfgidx) == USB_FAIL)
          {
            USB_CtlError();
            return;
          }
          USB_CtlSendStatus();
        }
        else
        {
          USB_CtlSendStatus();
        }
        break;

      default:
        USB_CtlError();
        break;
    }
  }
}

/**
* @brief  USB_GetConfig
*         Handle Get device configuration request
* @retval status
*/
static void USB_GetConfig(void)
{
  unsigned char config;

  if (USBHandle.Setup.req.wLength != 1)
  {
    USB_CtlError();
  }
  else
  {
    switch (USBHandle.dev_state )
    {
      case USB_STATE_ADDRESSED:
        config = 0;
        USB_CtlSendData ((unsigned char*)&config, 1);
        break;

      case USB_STATE_CONFIGURED:
        USB_CtlSendData ((unsigned char*)&USBHandle.dev_config, 1);
        break;

      default:
        USB_CtlError();
        break;
    }
  }
}

/**
* @brief  USB_GetStatus
*         Handle Get Status request
* @retval status
*/
static void USB_GetStatus(void)
{
  unsigned short dev_config_status;

  switch (USBHandle.dev_state)
  {
    case USB_STATE_ADDRESSED:
    case USB_STATE_CONFIGURED:

#ifdef USB_SELF_POWERED
      dev_config_status = USB_CONFIG_SELF_POWERED;
#else
      dev_config_status = 0;
#endif

      if (USBHandle.dev_remote_wakeup)
      {
        dev_config_status |= USB_CONFIG_REMOTE_WAKEUP;
      }
      USB_CtlSendData ((unsigned char*)&dev_config_status, 2);
      break;

    default :
      USB_CtlError();
      break;
  }
}

/**
* @brief  USB_SetFeature
*         Handle Set device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_SetFeature(void)
{
  if (USBHandle.Setup.req.wValue == USB_FEATURE_REMOTE_WAKEUP)
  {
    USBHandle.dev_remote_wakeup = 1;
    //USB_DEBUG("USB_SetFeature");
    //USBHandle.pClass->Setup();
    USB_CtlSendStatus();
  }
}


/**
* @brief  USB_ClrFeature
*         Handle clear device feature request
* @param  pdev: device instance
* @param  req: usb request
* @retval status
*/
static void USB_ClrFeature(void)
{
  switch (USBHandle.dev_state)
  {
    case USB_STATE_ADDRESSED:
    case USB_STATE_CONFIGURED:
      if (USBHandle.Setup.req.wValue == USB_FEATURE_REMOTE_WAKEUP)
      {
        USBHandle.dev_remote_wakeup = 0;
        //USB_DEBUG("USB_ClrFeature");
        //USBHandle.pClass->Setup();
        USB_CtlSendStatus();
      }
      break;

    default :
      USB_CtlError();
      break;
  }
}
