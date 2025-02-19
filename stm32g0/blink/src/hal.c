#include "board.h"
#include <systick.h>
#include <gpio.h>

volatile int Sys_Tick;

void SysTick_Handler(void)
{
    Sys_Tick++;
}

static void systick_start(unsigned int us)
{
    systick_set_reload(us * SYSTICK_MULTIPLIER);

    systick_interrupt_enable();
    /* start counting */
    systick_counter_enable();
}

static void systick_stop(void)
{
    /* stop counting */
    systick_counter_disable();
    systick_interrupt_disable();
}

static void systick_wait(void)
{
    Sys_Tick = 0;
    while (!Sys_Tick)
        __WFI();
}

void delay(unsigned int us)
{
    systick_start(us);
    systick_wait();
    systick_stop();
}

void delayms(unsigned int ms)
{
    while (ms--)
        delay(1000);
}

static void GPIOInit(void)
{
    //LED
    GPIO_Init(GPIOA,
              GPIO_Pin_4,
              GPIO_Mode_OUT,
              GPIO_Speed_Low,
              GPIO_OType_PP,
              GPIO_PuPd_NOPULL
    );
}

void SystemInit(void)
{
    /* 16MHz / 8 => 2M counts per second */
    systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

    //enable the GPIO clock for port GPIOA
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

    //enable the GPIO clock for port GPIOB
    //RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

    //enable the GPIO clock for port GPIOC
    //RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

    GPIOInit();
}

void _init(void)
{

}
