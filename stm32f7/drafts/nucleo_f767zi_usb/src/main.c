#include "board.h"
#include "usb_device_otg.h"
#include "class/cdc/usb_cdc.h"

static const USB_OTG_CfgTypeDef usbfs_cfg = {
  .instance = USB_OTG_FS,
  .dev_endpoints = 3,
  .vbus_sensing_enable = 1,
  .dev_remote_wakeup = 0,
  .dma_enable = 0,
  .Sof_enable = 0,
  .use_dedicated_ep1 = 0,
  .use_external_vbus = 0
};

static const USBDeviceConfiguration configuration =
{
  .device_class = usb_class_cdc,
  .vendor_id = 1155,
  .product_id =  22336,
  .manufacturer = "STMicroelectronics",
  .product = "STM32 Virtual ComPort",
  .serial_number = "00000000001A",
  .enabled_endpoints = otg_endpoints,
  .data = (void*)&usbfs_cfg,
  .Resume_Callback = NULL,
  .Suspend_Callback = NULL,
  .Connect_Callback = NULL,
  .Disconnect_Callback = NULL
};

static const USBConfigurationDescriptor configuration_descriptor = {
  .configuration_name = "CDC Configuration",
  .max_power = 100,
  .self_powered = 1,
  .remote_wakeup = 0
};

int led_state;

static void led_toggle(void)
{
  led_state = !led_state;
  if (led_state)
    LD1_ON;
  else
    LD1_OFF;
}

void cdc_rx_callback(unsigned int port_id, unsigned char *buffer, unsigned int buffer_length)
{

}

int main(void)
{
  led_state = 0;

  unsigned int device_id = USBDeviceInit(&configuration, &configuration_descriptor);
  USB_CDC_Init(device_id, 1, cdc_rx_callback);
  if (USBDeviceValidate(device_id))
  {
    while (1)
      ;
  }
  USBDeviceOtgInit(&usbfs_cfg);

  while (1)
  {
    delayms(1000);
    led_toggle();
  }
}
