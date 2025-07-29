#include "board.h"
#include <delay_systick.h>

int main(void)
{
  HalInit();

  while (1)
  {
    delayms(1000);
    LED_GREEN_TOGGLE;
  }
}
