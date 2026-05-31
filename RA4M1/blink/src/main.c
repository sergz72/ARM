#include "board.h"
#include <delay_systick.h>

int main(void)
{
  __asm volatile ("cpsie i" ::: "memory");
  while (1)
  {
    __WFI();
    if (timer_interrupt)
    {
      timer_interrupt = false;
      LED_TOGGLE;
    }
  }
}
