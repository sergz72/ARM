#include "board.h"
#include "usb_device_class.h"

extern USB_DEVICE_CLASS usb_device;

int gated_mode;

extern "C" {
  void USB_DRD_FS_IRQHandler(void)
  {
    usb_device.InterruptHandler();
  }

  void __attribute__((section(".RamFunc"))) TIM3_IRQHandler(void)
  {
    counter_high_counter++;
    TIM3->SR = 0;
  }

  void __attribute__((section(".RamFunc"))) TIM6_IRQHandler(void)
  {
    stop_counters();
    timer_event = 1;
    TIM6->SR = 0;
  }

  void __attribute__((section(".RamFunc"))) update_counters(void)
  {
    if (gated_mode)
    {
      counter_low = TIM2->CNT >> 16;
      counter_high = counter_high_counter;
      ExternalClockModeSet(TIM2);
      ExternalClockModeSet(TIM3);
    }
    else
    {
      counter_freq_low = TIM2->CNT;
      counter_freq_high = TIM3->CNT | (counter_high_counter << 16);
      GatedModeSet(TIM2);
      GatedModeSet(TIM3);
    }
    gated_mode = !gated_mode;
    counter_high_counter = 0;
  }

  void PeriodicTimerStart(void)
  {
    gated_mode = 1;
    TIM6->CR1 = TIM_CR1_CEN;
  }
}
