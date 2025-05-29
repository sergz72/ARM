#ifndef USB_DEVICE_OTG_H
#define USB_DEVICE_OTG_H

#include <usb_device.h>

typedef struct
{
  USB_OTG_GlobalTypeDef *instance;
  unsigned int dma_enable;           /*!< Enable or disable of the USB embedded DMA.                             */
  unsigned int Sof_enable;           /*!< Enable or disable the output of the SOF signal.                        */
  unsigned int use_dedicated_ep1;    /*!< Enable or disable the use of the dedicated EP1 interrupt.              */
  unsigned int vbus_sensing_enable;  /*!< Enable or disable the VBUS Sensing feature.                            */
  unsigned int use_external_vbus;    /*!< Enable or disable the use of the external VBUS.                        */
} USB_OTG_CfgTypeDef;

void USBDeviceOtgInit(const USB_OTG_CfgTypeDef *cfg, unsigned int dev_endpoints);
extern const char otg_endpoints[USB_MAX_ENDPOINTS];

#endif
