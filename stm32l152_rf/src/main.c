#include "board.h"
#include <st7066.h>
#include <clock_func.h>
#include <adc_func.h>
#include <th02_func.h>
#include <iwdg.h>
#include <cc1101_func.h>

/* LCD 20x4
 *
 * Su Nov 24 2019 23:13
 * In  -XX.X  Humid XX%
 * OuN -XX.X  OuW -XX.X
 * VBAT XXXX
 *
 */

void cc1101_GD0_Interrupt_Init(void);

extern volatile int second_interrupt;

int main(void)
{
  unsigned int thRc, ccRc;

  second_interrupt = 0;

  st7066_init(4, ST7066_ENTRY_MODE_ADDRESS_INCREMENT);
  st7066_display_control(ST7066_DISPLAY_ON);

  ClockInit();
  AdcInit();
  thRc = TH02Start();
  ccRc = cc1101Start();
  if (ccRc)
    cc1101_GD0_Interrupt_Init();

  while (1)
  {
    if (ccRc)
      cc1101Loop();
    if (second_interrupt)
    {
      second_interrupt = 0;
      /* Reload IWDG counter */
      IWDG_ReloadCounter();
      if (ClockLoop())
      {
        TH02Loop(thRc);
        AdcLoop();
      }
    }
    __WFI();
  }
}
