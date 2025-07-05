#include "board.h"
#include "usb_device_class.h"

extern USB_DEVICE_CLASS usb_device;

extern "C" {
  void USB_LP_IRQHandler(void)
  {
    usb_device.InterruptHandler();
  }

  void __attribute__((section(".RamFunc"))) update_counters(void)
  {
    counter_low = counter_low_counter;
    counter_high = counter_high_counter;
    counter_freq_low = TIM4->CNT | (freq_low_counter << 16);
    counter_freq_high = TIM3->CNT | (freq_high_counter << 16);
    counter_low_counter = counter_high_counter = freq_low_counter = freq_high_counter = 0;
  }

  void __attribute__((section(".RamFunc"))) TIM2_IRQHandler(void)
  {
    stop_counters();
    timer_event = 1;
    TIM2->SR = 0;
  }

  void __attribute__((section(".RamFunc"))) TIM4_IRQHandler(void)
  {
    freq_low_counter++;
    TIM4->SR = 0;
  }

  void __attribute__((section(".RamFunc"))) TIM3_IRQHandler(void)
  {
    freq_high_counter++;
    TIM3->SR = 0;
  }

  void __attribute__((section(".RamFunc"))) TIM1_UP_TIM16_IRQHandler(void)
  {
    counter_low_counter++;
    TIM1->SR = 0;
  }

  void __attribute__((section(".RamFunc"))) TIM8_UP_IRQHandler(void)
  {
    counter_high_counter++;
    TIM8->SR = 0;
  }

  void PeriodicTimerStart(void)
  {
    TIM2->CR1 = TIM_CR1_CEN;
  }
}
