#include "board.h"
#include <system.h>
#include <interrupts.h>

volatile bool timer_interrupt;
static unsigned int timer_interrupt_no;

void GPT0_Interrupt_Handler(void)
{
  R_ICU->IELSR[0] &= ~(1 << (16 + timer_interrupt_no));
  timer_interrupt = true;
}

static void ClockInit(void)
{
  R_SYSTEM->PRCR = 0xA501; // Enable writing to the clock registers
  R_SYSTEM->HOCOCR2 = 0; // 24 MHz
  R_SYSTEM->HOCOCR = 0; // enabled
  R_SYSTEM->MEMWAIT = 0; // 0 wait cycles
  // switch to HOCO (48 MHz)
  R_SYSTEM->SCKSCR = 0;
  R_SYSTEM->SCKDIVCR_b.ICK = 6; // divider:64
  R_SYSTEM->SCKDIVCR_b.FCK = 6; // divider:64
  R_SYSTEM->SCKDIVCR_b.PCKA = 6; // divider:64
  R_SYSTEM->SCKDIVCR_b.PCKB = 6; // divider:64
  R_SYSTEM->SCKDIVCR_b.PCKC = 6; // divider:64
  R_SYSTEM->SCKDIVCR_b.PCKD = 6; // divider:64
  // invalidate flash cache
  R_FCACHE->FCACHEIV = 1;
  while (R_FCACHE->FCACHEIV & 1)
    ;
  // enable flash cache
  R_FCACHE->FCACHEE = 1;
  R_SYSTEM->PRCR = 0xA500; // Disable writing to the clock registers

  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
}

static void GPIOInit(void)
{
  /* Set P012 as output */
  R_PORT0->PDR |= (1 << LED_PIN);   // Direction: Output
}

static void GPT0Init(void)
{
  // cancel GPT0 stop state
  R_MSTP->MSTPCRD &= ~(1 << 5);

  R_GPT0->GTWP = 0x5A00; // enable registers write

  R_GPT0->GTUDDTYC = 1; //Count UP
  R_GPT0->GTPR = PCKD_FREQUENCY/TIMER_INTERRUPT_FREQUENCY - 1;

  timer_interrupt_no = RegisterApplicationInterrupt(GPT0_Interrupt_Handler, ELC_EVENT_GPT0_COUNTER_OVERFLOW);

  NVIC_SetPriority(timer_interrupt_no, 3);
  NVIC_EnableIRQ(timer_interrupt_no);

  R_GPT0->GTCR = 1; // start timer

  R_GPT0->GTWP = 0x5A01; // disable registers write
}

void SystemInit(void)
{
  timer_interrupt = false;

  ClockInit();
  SystemRuntimeInit(0);
  GPIOInit();
  GPT0Init();
}

void _init(void)
{

}
