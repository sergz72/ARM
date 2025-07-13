#include "board.h"
#include "usb_device_class.h"

extern USB_DEVICE_CLASS usb_device;

extern "C" {
  void USB_DRD_FS_IRQHandler(void)
  {
    usb_device.InterruptHandler();
  }

  void __attribute__((section(".RamFunc"))) update_counters(void)
  {
    //todo
    counter_low = counter_low_counter;
    counter_high = counter_high_counter;
    counter_freq_low = 0 | (freq_low_counter << 16);
    counter_freq_high = 0 | (freq_high_counter << 16);
    counter_low_counter = counter_high_counter = freq_low_counter = freq_high_counter = 0;
  }

  void PeriodicTimerStart(void)
  {
    //todo
  }
}
