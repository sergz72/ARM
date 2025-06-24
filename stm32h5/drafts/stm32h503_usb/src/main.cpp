#include "board.h"
#include <delay_systick.h>
#include <usb_device.h>
#include <usb_device_drd.h>
#include <cdc_class.h>

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
  .self_powered = 1,
  .remote_wakeup = 0,
  .max_power = 100
};

extern "C" {
static void cdc_rx_callback(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length);
}

static int led_state;
static USB_Device_DRD usb_device;
static USB_DeviceManager usb_device_manager(&configuration, &configuration_descriptor, &usb_device);
static USB_CDC_Class cdc_class(&usb_device_manager, cdc_rx_callback);

extern "C" {
  void USB_DRD_FS_IRQHandler(void)
  {
    usb_device.InterruptHandler();
  }

  static void led_toggle(void)
  {
    led_state = !led_state;
    if (led_state)
      LED_ON;
    else
      LED_OFF;
  }

  static void cdc_rx_callback(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length)
  {

  }

  int main(void)
  {
    led_state = 0;

    if (cdc_class.DescriptorBuilder(1) || usb_device_manager.Init())
    {
      LED_ON;
      while (1)
        delayms(1000);
    }

    while (1)
    {
      delayms(1000);
      led_toggle();
    }
  }
}
