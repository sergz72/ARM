#include <board.h>
#include <usb_desc.h>
#include <usb.h>
#include <usb_cdc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* USB CDC device Configuration Descriptor */
const __ALIGN_BEGIN unsigned char USB_CfgFSDesc[USB_DESC_SIZ] __ALIGN_END =
  {
    USB_CONFIG_DESC(2)
    CDC_INTERFACE_DESC(0, 1, 2, 1),
  };

/** Received data over USB are stored in this buffer      */
__ALIGN_BEGIN unsigned char UserRxBufferFS1[CDC_RX_DATA_SIZE] __ALIGN_END;

USB_CDC_HandleTypeDef cdc_handle1;

const USB_OTG_CfgTypeDef USBCfg = {
  5, //Device Endpoints number.
  0, //Enable or disable of the USB embedded DMA.
  0, //Enable or disable the output of the SOF signal.
  0, //Enable or disable the low power mode.
  0, //Enable or disable Link Power Management.
  0,
  0,
  { NULL, &cdc_handle1, &cdc_handle1 },
  { &cdc_handle1, &cdc_handle1 },
  { &cdc_handle1 }
};

void USB_FIFO_Init(void)
{
  unsigned int i;

  USB_SetRxFiFo(0x40);
  USB_SetTxFiFo(0, 0x40);
  for (i = 1; i < USBHandle.Cfg->dev_endpoints; i++)
    USB_SetTxFiFo(i, 0x40);
}

static void serial_receive1(unsigned char *buf, unsigned int len)
{
  while (len--)
    queue_push(&usb_q, buf++);
}

void USBStart(void)
{
  cdc_handle1.RxBuffer = UserRxBufferFS1;
  cdc_handle1.serial_receive = serial_receive1;
  cdc_handle1.is_initialized = 0;
  cdc_handle1.data_endpoint_no = 1;
  cdc_handle1.control_endpoint_no = 2;
  USBInit(USB_OTG_FS, &USBCfg, &Default_Callbacks, &CDC_Class, &USB_Desc);
}

int usb_printf(const char *format, ...)
{
  static char buffer[PRINTF_BUFFER_LENGTH], buffer2[PRINTF_BUFFER_LENGTH];
  char *p, *p2;
  va_list vArgs;
  int rc;

  va_start(vArgs, format);
  rc = vsnprintf(buffer, sizeof(buffer), format, vArgs);
  va_end(vArgs);
  p = buffer;
  p2 = buffer2;
  while (*p)
  {
    if (*p == '\n')
      *p2++ = '\r';
    *p2++ = *p++;
  }
  *p2 = 0;
  puts_(buffer2);
  return rc;
}

void puts_(const char *s)
{
  while (CDC_Transmit(1, s, strlen(s)) != USB_OK)
    ;
}
