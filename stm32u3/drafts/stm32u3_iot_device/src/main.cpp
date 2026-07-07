#include "board.h"
#include <delay_systick.h>
#include <usb_device.h>
#include <usb_device_stm32f.h>
#include <cdc_class.h>
#include <cstdlib>

static const USBDeviceConfiguration configuration =
{
  .device_class = usb_class_cdc,
  .vendor_id = 1155,
  .product_id =  22336,
  .manufacturer = "STMicroelectronics",
  .product = "STM32 Virtual ComPort",
  .serial_number = "00000000001A",
  .language_id = 1033
};

static const USBConfigurationDescriptor configuration_descriptor = {
  .configuration_name = "CDC Configuration",
  .self_powered = true,
  .remote_wakeup = false,
  .max_power = 100
};

const USB_OTG_CfgTypeDef device_cfg =  {
  .vbus_sensing_enable = false,
  .use_external_vbus = false
};

static int led_state;
static USB_Device_STM32F usb_device(USB_OTG_FS, &device_cfg);
static USB_DeviceManager usb_device_manager(&configuration, &configuration_descriptor, &usb_device);
static USB_CDC_Class cdc_class(&usb_device_manager, 1024);
static unsigned char cdc_buffer[1024];

extern "C" {
  void __libc_init_array(void);

  void OTG_FS_IRQHandler(void)
  {
    usb_device.InterruptHandler();
  }

  static void led_toggle(void)
  {
    led_state = !led_state;
    if (led_state)
      LED_GREEN_ON;
    else
      LED_GREEN_OFF;
  }

  int main(void)
  {
    __libc_init_array();

    led_state = 0;
    int cnt = 0;

    if (cdc_class.DescriptorBuilder(1, false) || usb_device_manager.Init() || !malloc(1))
    {
      LED_RED_ON;
      while (1)
        delayms(1000);
    }

    while (1)
    {
      delayms(10);
      unsigned int length = cdc_class.GetPendingData(0, cdc_buffer, sizeof(cdc_buffer));
      if (length)
        cdc_class.Send(0, cdc_buffer, length);
      cnt++;
      if (cnt == 100)
      {
        led_toggle();
        cnt = 0;
      }
    }
  }
}
