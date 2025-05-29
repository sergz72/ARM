#ifndef USB_DEVICE_OTG_H
#define USB_DEVICE_OTG_H

#include <usb_device.h>

typedef struct
{
  USB_OTG_GlobalTypeDef *instance;
  unsigned int dev_endpoints;        /*!< Device Endpoints number.
                                        This parameter depends on the used USB core.
                                        This parameter must be a number between Min_Data = 1 and Max_Data = 15 */
  unsigned int dma_enable;           /*!< Enable or disable of the USB embedded DMA.                             */
  unsigned int Sof_enable;           /*!< Enable or disable the output of the SOF signal.                        */
  unsigned int use_dedicated_ep1;    /*!< Enable or disable the use of the dedicated EP1 interrupt.              */
  unsigned int vbus_sensing_enable;  /*!< Enable or disable the VBUS Sensing feature.                            */
  unsigned int use_external_vbus;    /*!< Enable or disable the use of the external VBUS.                        */
  unsigned int dev_remote_wakeup;
} USB_OTG_CfgTypeDef;

void USBDeviceOtgInit(const USB_OTG_CfgTypeDef *cfg);
extern const char otg_endpoints[USB_MAX_ENDPOINTS];

#endif
