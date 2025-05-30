#ifndef USB_DEVICE_OTG_H
#define USB_DEVICE_OTG_H

#include <usb_device.h>

typedef struct
{
  unsigned char *buffer;
  unsigned int xfer_len;
} USB_OTG_XferBuffer;

typedef struct
{
  USB_OTG_GlobalTypeDef *instance;
  unsigned int dma_enable;           /*!< Enable or disable of the USB embedded DMA.                             */
  unsigned int Sof_enable;           /*!< Enable or disable the output of the SOF signal.                        */
  unsigned int use_dedicated_ep1;    /*!< Enable or disable the use of the dedicated EP1 interrupt.              */
  unsigned int vbus_sensing_enable;  /*!< Enable or disable the VBUS Sensing feature.                            */
  unsigned int use_external_vbus;    /*!< Enable or disable the use of the external VBUS.                        */
  unsigned int dev_endpoints;
  USB_OTG_XferBuffer in_xfer_buffers[USB_MAX_ENDPOINTS];
  USB_OTG_XferBuffer out_xfer_buffers[USB_MAX_ENDPOINTS];
} USB_OTG_CfgTypeDef;

int USBDeviceOtgInit(USB_OTG_CfgTypeDef *cfg, unsigned int dev_endpoints, unsigned int *xfer_buffers,
                      int xfer_buffers_length, int *endpoint_packet_lengths);
extern const char otg_endpoints[USB_MAX_ENDPOINTS];

#endif
