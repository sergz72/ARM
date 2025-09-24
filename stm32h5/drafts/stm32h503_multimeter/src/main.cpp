#include "board.h"
#include "usb_device.h"
#include <cdc_class.h>
#include <cstdlib>
#include <cstring>
#include <delay_systick.h>
#include <usb_device_drd.h>

static const USBDeviceConfiguration configuration =
{
  .device_class = usb_class_misc, // composite device
  .vendor_id = 1155,
  .product_id =  22336,
  .manufacturer = "STMicroelectronics",
  .product = "STM32 Virtual ComPort",
  .serial_number = "00000000001A",
  .language_id = 1033
};

static const USBConfigurationDescriptor configuration_descriptor = {
  .configuration_name = "CDC Configuration",
  .self_powered = 1,
  .remote_wakeup = 0,
  .max_power = 100
};

USB_Device_DRD usb_device;
static USB_DeviceManager usb_device_manager(&configuration, &configuration_descriptor, &usb_device);
static USB_CDC_Class cdc_class(&usb_device_manager, 1024);

extern "C" {
  int getch_(void)
  {
    return cdc_class.GetPendingChar(0);
  }

  void puts_(const char *s)
  {
    cdc_class.Send(0, (unsigned char*)s, strlen(s));
  }

  void CDCInit(void)
  {
    if (cdc_class.DescriptorBuilder(1, false) || usb_device_manager.Init() || !malloc(1))
    {
      LED_ON;
      while (1)
        delayms(1000);
    }
  }

  void USB_DRD_FS_IRQHandler(void)
  {
    usb_device.InterruptHandler();
  }
}
