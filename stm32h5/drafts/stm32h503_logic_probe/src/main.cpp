#include "board.h"
#include "usb_device_class.h"

extern USB_DEVICE_CLASS usb_device;

extern "C" {
  void USB_DRD_FS_IRQHandler(void)
  {
    usb_device.InterruptHandler();
  }

  void __attribute__((section(".RamFunc"))) TIM2_IRQHandler(void)
  {
    stop_counters();
    timer_event = 1;
    TIM2->SR = 0;
  }

  void __attribute__((section(".RamFunc"))) update_counters(void)
  {
    //todo
    counter_low = COUNTERS_MAX / 2;//counter_low_counter;
    counter_high = COUNTERS_MAX / 3;//counter_high_counter;
    counter_z = COUNTERS_MAX / 4;
    counter_freq_low = 0 | (freq_low_counter << 16);
    counter_freq_high = 0 | (freq_high_counter << 16);
    counter_freq_rs = 1;//0 | (freq_rs_counter << 16);
    counter_low_counter = 0;
    counter_high_counter = 0;
    counter_z_counter = 0;
    freq_low_counter = 0;
    freq_high_counter = 0;
    freq_rs_counter = 0;
  }

  void PeriodicTimerStart(void)
  {
    TIM2->CR1 = TIM_CR1_CEN;
  }
}
