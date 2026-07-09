#include <usb_audio.h>
#include <string.h>

#define AUDIO_SAMPLE_FREQ(frq)      (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))

#define AUDIO_PACKET_SZE(frq)          (uint8_t)(((frq * 2 * 2)/1000) & 0xFF), \
                                       (uint8_t)((((frq * 2 * 2)/1000) >> 8) & 0xFF)
                                         
static unsigned int  USB_AUDIO_Init (unsigned int cfgidx);

static void  USB_AUDIO_DeInit (unsigned int cfgidx);

static void  USB_AUDIO_Setup (unsigned int interface_no);

static void  USB_AUDIO_DataIn (unsigned int epnum);

static void  USB_AUDIO_DataOut (unsigned int epnum);

static void  USB_AUDIO_EP0_RxReady (void);

static void AUDIO_REQ_GetCurrent(void);

static void AUDIO_REQ_SetCurrent(void);

USB_ClassTypeDef  USB_AUDIO =
{
  USB_AUDIO_Init,
  USB_AUDIO_DeInit,
  USB_AUDIO_Setup,
  NULL,
  USB_AUDIO_EP0_RxReady,
  USB_AUDIO_DataIn,
  USB_AUDIO_DataOut,
  NULL,
  NULL,
  NULL
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_AUDIO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END=
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */ 

/** @defgroup USBD_AUDIO_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_AUDIO_Init
  *         Initialize the AUDIO interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static unsigned int  USB_AUDIO_Init (unsigned int cfgidx)
{
  unsigned int i;
  USB_AUDIO_HandleTypeDef   *haudio;

  for (i = 0; i < USB_EP_MAX; i++)
  {
    haudio = USBHandle.Cfg->pAUDIOClassData[i];
    if (!haudio)
      return 0;
    if (haudio->is_initialized)
      continue;

    /* Open EP IN */
    USB_EP_Open(1, haudio->data_endpoint_no,
                USB_EP_TYPE_ISOC);

    /* Open EP OUT */
    USB_EP_Open(0, haudio->data_endpoint_no,
                USB_EP_TYPE_ISOC);

    haudio->alt_setting = 0;
    haudio->offset = AUDIO_OFFSET_UNKNOWN;
    haudio->wr_ptr = 0;
    haudio->rd_ptr = 0;
    haudio->rd_enable = 0;

    /* Initialize the Audio output Hardware layer */
    if (((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->Init(USBD_AUDIO_FREQ, AUDIO_DEFAULT_VOLUME, 0) != USBD_OK)
      return 1;

    /* Prepare Out endpoint to receive next packet */
    USB_EP_Receive(haudio->data_endpoint_no,
                   haudio->buffer,
                   USB_FS_MAX_PACKET_SIZE);
  }

  return 0;
}

/**
  * @brief  USBD_AUDIO_Init
  *         DeInitialize the AUDIO layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static void  USB_AUDIO_DeInit (unsigned int cfgidx)
{
  unsigned int i;
  USB_AUDIO_HandleTypeDef   *haudio;

  for (i = 0; i < USB_EP_MAX; i++)
  {
    haudio = USBHandle.Cfg->pAUDIOClassData[i];
    if (!haudio)
      return;
    if (!haudio->is_initialized)
      continue;

    USB_DeactivateEndpoint(1, haudio->data_endpoint_no);
    USB_DeactivateEndpoint(0, haudio->data_endpoint_no);

    /* DeInit  physical Interface components */
    if(pdev->pClassData != NULL)
    {
      ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->DeInit(0);
      USBD_free(pdev->pClassData);
      pdev->pClassData = NULL;
    }
  }

  return 0;
}

/**
  * @brief  USBD_AUDIO_Setup
  *         Handle the AUDIO specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static void USBD_AUDIO_Setup (unsigned int interface_no)
{
  USB_AUDIO_HandleTypeDef   *haudio = (USB_AUDIO_HandleTypeDef*) USBHandle.Cfg->pItfClassData[interface_no];

  switch (USBHandle.Setup.req.bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS :  
    switch (USBHandle.Setup.req.bRequest)
    {
    case AUDIO_REQ_GET_CUR:
      AUDIO_REQ_GetCurrent();
      break;
      
    case AUDIO_REQ_SET_CUR:
      AUDIO_REQ_SetCurrent();
      break;
      
    default:
      USB_CtlError();
    }
    break;
    
  case USB_REQ_TYPE_STANDARD:
    switch (USBHandle.Setup.req.bRequest)
    {
    case USB_REQ_GET_INTERFACE :
      USB_CtlSendData ((uint8_t *)&(haudio->alt_setting),
                        1);
      break;
      
    case USB_REQ_SET_INTERFACE :
      if ((uint8_t)(USBHandle.Setup.req.wValue) <= USB_MAX_NUM_INTERFACES)
      {
        haudio->alt_setting = (uint8_t)(USBHandle.Setup.req.wValue);
      }
      else
      {
        /* Call the error management function (command will be nacked */
        USB_CtlError();
      }
      break;      
      
    default:
      USB_CtlError();
    }
  }

}

/**
  * @brief  USBD_AUDIO_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static void  USB_AUDIO_DataIn (unsigned int epnum)
{
  /* Only OUT data are processed */
}

/**
  * @brief  USBD_AUDIO_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static void USB_AUDIO_EP0_RxReady(void)
{
  unsigned int i;
  USB_AUDIO_HandleTypeDef   *haudio;

  for (i = 0; i < USB_EP_MAX; i++)
  {
    haudio = USBHandle.Cfg->pAUDIOClassData[i];
    if (!haudio)
      return;

    if (haudio->control.cmd == AUDIO_REQ_SET_CUR)
    {/* In this driver, to simplify code, only SET_CUR request is managed */

      if (haudio->control.unit == AUDIO_OUT_STREAMING_CTRL)
      {
        ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->MuteCtl(haudio->control.data[0]);
        haudio->control.cmd = 0;
        haudio->control.len = 0;
      }
    }
  }
}

/**
  * @brief  USBD_AUDIO_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
void  USBD_AUDIO_Sync (USBD_HandleTypeDef *pdev, AUDIO_OffsetTypeDef offset)
{
  int8_t shift = 0;
  USBD_AUDIO_HandleTypeDef   *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;
  
  haudio->offset =  offset; 
  
  
  if(haudio->rd_enable == 1)
  {
    haudio->rd_ptr += AUDIO_TOTAL_BUF_SIZE/2;
    
    if (haudio->rd_ptr == AUDIO_TOTAL_BUF_SIZE)
    {
      /* roll back */
      haudio->rd_ptr = 0;
    }
  }
  
  if(haudio->rd_ptr > haudio->wr_ptr)
  {
    if((haudio->rd_ptr - haudio->wr_ptr) < AUDIO_OUT_PACKET)
    {
      shift = -4;
    }
    else if((haudio->rd_ptr - haudio->wr_ptr) > (AUDIO_TOTAL_BUF_SIZE - AUDIO_OUT_PACKET))
    {
      shift = 4;
    }    

  }
  else
  {
    if((haudio->wr_ptr - haudio->rd_ptr) < AUDIO_OUT_PACKET)
    {
      shift = 4;
    }
    else if((haudio->wr_ptr - haudio->rd_ptr) > (AUDIO_TOTAL_BUF_SIZE - AUDIO_OUT_PACKET))
    {
      shift = -4;
    }  
  }

  if(haudio->offset == AUDIO_OFFSET_FULL)
  {
    ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->AudioCmd(&haudio->buffer[0],
                                                         AUDIO_TOTAL_BUF_SIZE/2 - shift,
                                                         AUDIO_CMD_PLAY); 
      haudio->offset = AUDIO_OFFSET_NONE;           
  }
}

/**
  * @brief  USBD_AUDIO_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static void USBD_AUDIO_DataOut (unsigned int epnum)
{
  USB_AUDIO_HandleTypeDef *haudio = (USB_AUDIO_HandleTypeDef*) USBHandle.Cfg->pEpClassData[epnum];

  /* Increment the Buffer pointer or roll it back when all buffers are full */

  haudio->wr_ptr += AUDIO_OUT_PACKET;

  if (haudio->wr_ptr == AUDIO_TOTAL_BUF_SIZE)
  {/* All buffers are full: roll back */
    haudio->wr_ptr = 0;

    if(haudio->offset == AUDIO_OFFSET_UNKNOWN)
    {
      ((USBD_AUDIO_ItfTypeDef *)pdev->pUserData)->AudioCmd(&haudio->buffer[0],
                                                           AUDIO_TOTAL_BUF_SIZE/2,
                                                           AUDIO_CMD_START);
        haudio->offset = AUDIO_OFFSET_NONE;
    }
  }

  if(haudio->rd_enable == 0)
  {
    if (haudio->wr_ptr == (AUDIO_TOTAL_BUF_SIZE / 2))
    {
      haudio->rd_enable = 1;
    }
  }

  /* Prepare Out endpoint to receive next audio packet */
  USB_EP_Receive(epnum,
                         &haudio->buffer[haudio->wr_ptr],
                         AUDIO_OUT_PACKET);
}

/**
  * @brief  AUDIO_Req_GetCurrent
  *         Handles the GET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_REQ_GetCurrent(void)
{  
  USBD_AUDIO_HandleTypeDef   *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;
  
  memset(haudio->control.data, 0, 64);
  /* Send the current mute state */
  USB_CtlSendData (haudio->control.data,
                    USBHandle.Setup.req.wLength);
}

/**
  * @brief  AUDIO_Req_SetCurrent
  *         Handles the SET_CUR Audio control request.
  * @param  pdev: instance
  * @param  req: setup class request
  * @retval status
  */
static void AUDIO_REQ_SetCurrent(void)
{ 
  USBD_AUDIO_HandleTypeDef   *haudio;
  haudio = (USBD_AUDIO_HandleTypeDef*) pdev->pClassData;
  
  if (USBHandle.Setup.req.wLength)
  {
    /* Prepare the reception of the buffer over EP0 */
    USB_CtlPrepareRx (haudio->control.data,
                       USBHandle.Setup.req.wLength);
    
    haudio->control.cmd = AUDIO_REQ_SET_CUR;     /* Set the request value */
    haudio->control.len = USBHandle.Setup.req.wLength;          /* Set the request data length */
    haudio->control.unit = HIBYTE(USBHandle.Setup.req.wIndex);  /* Set the request target unit */
  }
}


/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_AUDIO_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_AUDIO_DeviceQualifierDesc);
  return USBD_AUDIO_DeviceQualifierDesc;
}

/**
* @brief  USBD_AUDIO_RegisterInterface
* @param  fops: Audio interface callback
* @retval status
*/
uint8_t  USBD_AUDIO_RegisterInterface(USBD_AUDIO_ItfTypeDef *fops)
{
  if(fops != NULL)
  {
    pdev->pUserData= fops;
  }
  return 0;
}
/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
