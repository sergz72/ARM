#include <usb.h>

#define USB_MAX_STR_DESC_SIZ                 0x100

unsigned char * USB_FS_DeviceDescriptor(unsigned int *length);
unsigned char * USB_FS_LangIDStrDescriptor(unsigned int *length);
unsigned char * USB_FS_ManufacturerStrDescriptor(unsigned int *length);
unsigned char * USB_FS_ProductStrDescriptor(unsigned int *length);
unsigned char * USB_FS_SerialStrDescriptor(unsigned int *length);
unsigned char * USB_FS_ConfigStrDescriptor(unsigned int *length);
unsigned char * USB_FS_InterfaceStrDescriptor(unsigned int *length);

#ifdef USB_SUPPORT_USER_STRING_DESC
unsigned char * USBD_FS_USRStringDesc(unsigned int idx, unsigned int *length);
#endif /* USB_SUPPORT_USER_STRING_DESC */

const USB_DescriptorsTypeDef USB_Desc =
{
  USB_FS_DeviceDescriptor
, USB_FS_LangIDStrDescriptor
, USB_FS_ManufacturerStrDescriptor
, USB_FS_ProductStrDescriptor
, USB_FS_SerialStrDescriptor
, USB_FS_ConfigStrDescriptor
, USB_FS_InterfaceStrDescriptor
};

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
/** USB standard device descriptor. */
const __ALIGN_BEGIN unsigned char USB_FS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
  0x12,                       /*bLength */
  USB_DESC_TYPE_DEVICE,       /*bDescriptorType*/
  0x00,                       /*bcdUSB */
  0x02,
#ifdef USB_COMPOSITE
  0xEF,
#else
  USB_DEVICECLASS,                       /*bDeviceClass*/
#endif
  0x02,                       /*bDeviceSubClass*/
#ifdef USB_COMPOSITE
  0x01,
#else
  0x00,                       /*bDeviceProtocol*/
#endif
  USB_MAX_EP0_SIZE,           /*bMaxPacketSize*/
  LOBYTE(USB_VID),           /*idVendor*/
  HIBYTE(USB_VID),           /*idVendor*/
  LOBYTE(USB_PID_FS),        /*idProduct*/
  HIBYTE(USB_PID_FS),        /*idProduct*/
  0x00,                       /*bcdDevice rel. 2.00*/
  0x02,
  USB_IDX_MFC_STR,           /*Index of manufacturer  string*/
  USB_IDX_PRODUCT_STR,       /*Index of product string*/
  USB_IDX_SERIAL_STR,        /*Index of serial number string*/
  USB_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/
};

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */

/** USB lang indentifier descriptor. */
const __ALIGN_BEGIN unsigned char USB_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END =
{
     USB_LEN_LANGID_STR_DESC,
     USB_DESC_TYPE_STRING,
     LOBYTE(USB_LANGID_STRING),
     HIBYTE(USB_LANGID_STRING)
};

#if defined ( __ICCARM__ ) /* IAR Compiler */
  #pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
/* Internal string descriptor. */
__ALIGN_BEGIN unsigned char USB_StrDesc[USB_MAX_STR_DESC_SIZ] __ALIGN_END;

/**
  * @}
  */

/** @defgroup USBD_DESC_Private_Functions USBD_DESC_Private_Functions
  * @brief Private functions.
  * @{
  */

/**
  * @brief  Return the device descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
unsigned char * USB_FS_DeviceDescriptor(unsigned int *length)
{
  *length = sizeof(USB_FS_DeviceDesc);
  return (unsigned char*)USB_FS_DeviceDesc;
}

/**
  * @brief  Return the LangID string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
unsigned char * USB_FS_LangIDStrDescriptor(unsigned int *length)
{
  *length = sizeof(USB_LangIDDesc);
  return (unsigned char*)USB_LangIDDesc;
}

/**
  * @brief  Return the product string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
unsigned char * USB_FS_ProductStrDescriptor(unsigned int *length)
{
  USB_GetString((unsigned char *)USB_PRODUCT_STRING_FS, USB_StrDesc, length);
  return USB_StrDesc;
}

/**
  * @brief  Return the manufacturer string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
unsigned char * USB_FS_ManufacturerStrDescriptor(unsigned int *length)
{
  USB_GetString((unsigned char *)USB_MANUFACTURER_STRING, USB_StrDesc, length);
  return USB_StrDesc;
}

/**
  * @brief  Return the serial number string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
unsigned char * USB_FS_SerialStrDescriptor(unsigned int *length)
{
  USB_GetString((unsigned char *)USB_SERIAL_NUMBER_STRING, USB_StrDesc, length);
  return USB_StrDesc;
}

/**
  * @brief  Return the configuration string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
unsigned char * USB_FS_ConfigStrDescriptor(unsigned int *length)
{
  USB_GetString((unsigned char *)USB_CONFIGURATION_STRING_FS, USB_StrDesc, length);
  return USB_StrDesc;
}

/**
  * @brief  Return the interface string descriptor
  * @param  speed : Current device speed
  * @param  length : Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
unsigned char * USB_FS_InterfaceStrDescriptor(unsigned int *length)
{
  USB_GetString((unsigned char *)USB_INTERFACE_STRING_FS, USB_StrDesc, length);
  return USB_StrDesc;
}
