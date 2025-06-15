#include "board.h"
#include <systick.h>
#include <gpio.h>
#include <nvic.h>
#include <string.h>
#include <usart.h>

static void GPIOInit(void)
{
  //LED
  GPIO_Init(GPIOC,
            GPIO_Pin_6,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  //Button
  GPIO_Init(GPIOC,
            GPIO_Pin_13,
            GPIO_Mode_IN,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_DOWN
  );

  //BOOT0 Button
  GPIO_Init(GPIOB,
            GPIO_Pin_8,
            GPIO_Mode_IN,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  //LEDs
  GPIO_Init(GPIOB,
            GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

/*
 * PA2=USART2_TX
 * PA3=USART2_RX
 */

static void USART2Init(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
  GPIO_Init(GPIOA,
            GPIO_Pin_2 | GPIO_Pin_3,
            GPIO_Mode_AF,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_UP
  );

  USART2->BRR = BOARD_PCLK1 / USART_BAUD_RATE;
  //USART2->CR2 = 0;
  //USART2->CR3 = USART_CR3_DMAT;
  // Transmitter enable, Receiver enable, FIFO enable, RXFIFO not empty interrupt enable
  USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_FIFOEN | USART_CR1_RXNEIE;
  // USART enable in low-power mode, USART enable
  USART2->CR1 |= USART_CR1_UE | USART_CR1_UESM;

  NVIC_Init(USART2_IRQn, USART_INTERRUPT_PRIORITY, 0, ENABLE);
}

unsigned int mv_to_12(unsigned int mv)
{
  if (mv >= DAC_REFERENCE_VOLTAGE)
    return 4095;
  return (mv * 4095) / DAC_REFERENCE_VOLTAGE;
}

// PA5 = DAC1_OUT2
static void DACSInit(void)
{
  RCC->AHB2ENR |= RCC_AHB2ENR_DAC1EN;
  RCC->AHB2ENR |= RCC_AHB2ENR_DAC3EN;

  GPIO_Init(GPIOA,
            GPIO_Pin_5,
            GPIO_Mode_AN,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  DAC1->DHR12R1 = mv_to_12(DEFAULT_DAC1_VOLTAGE);
  DAC1->DHR12R2 = mv_to_12(DEFAULT_DAC1_VOLTAGE);
  //DAC channel1 is connected to on chip peripherals with Buffer disabled
  //DAC channel2 is connected to external pin with Buffer enabled
  DAC1->MCR = DAC_MCR_MODE1;
  DAC1->CR = DAC_CR_EN1 | DAC_CR_EN2;

  DAC3->DHR12R1 = mv_to_12(DEFAULT_DAC3_VOLTAGE);
  //DAC channel1 is connected to on chip peripherals with Buffer disabled
  DAC3->MCR = DAC_MCR_MODE1;
  DAC3->CR = DAC_CR_EN1;
}

// PA0 = COMP3_INP
// PA1 = COMP1_INP
static void ComparatorsInit(void)
{
  GPIO_Init(GPIOA,
            GPIO_Pin_0 | GPIO_Pin_1,
            GPIO_Mode_AN,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  // INP = PA1, INM = DAC1_CH1, inverted, hysteresis 70 mv
  COMP1->CSR = COMP_CSR_EN | COMP_CSR_INMSEL_2 | COMP_CSR_INMSEL_0 | COMP_CSR_POLARITY | COMP_CSR_HYST;
  // INP = PA0, INM = DAC3_CH1, not inverted, hysteresis 70 mv
  COMP3->CSR = COMP_CSR_EN | COMP_CSR_INMSEL_2 | COMP_CSR_HYST;
}

static void TIM1Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  // Route COMP1 output to TIM1 via interconnect matrix
  // tim_ti1 = tim_ti1_in1 (comp1_out)
  TIM1->TISEL = 1;

  // Configure TIM1 in gated mode
  TIM1->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_2 |                  // Gated mode
               TIM_SMCR_TS_0;    // tim_itr1 as trigger source

  TIM1->SR = 0;
  NVIC_Init(TIM1_UP_TIM16_IRQn, COUNTERS_INTERRUPT_PRIORITY, 0, ENABLE);
}

static void TIM8Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
  //todo
  TIM8->SR = 0;
  NVIC_Init(TIM8_UP_IRQn, COUNTERS_INTERRUPT_PRIORITY, 0, ENABLE);
}

void stop_counters(void)
{
  TIM1->CR1 &= ~TIM_CR1_CEN;
  TIM8->CR1 &= ~TIM_CR1_CEN;
}

void start_counters(void)
{
  TIM1->CNT = 0;
  TIM8->CNT = 0;
  TIM1->CR1 |= TIM_CR1_CEN;
  TIM8->CR1 |= TIM_CR1_CEN;
}

void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty)
{
  unsigned int arr = BOARD_PCLK1 / frequency;
  if (arr < 2)
    arr = 2;
  if (arr > 65536)
    arr = 65536;
  if (duty == 0)
    duty = 1;
  else if (duty > 99)
    duty = 99;
  TIM3->ARR = arr - 1;
  TIM3->CCR3 = arr * duty / 100;
}

// PWM timer
static void TIM3Init(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

  GPIO_Init(GPIOB,
            GPIO_Pin_0,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_2);

  pwm_set_frequency_and_duty(10000, 50);

  TIM3->CCMR2 = (6 << TIM_CCMR2_OC3M_Pos);  // PWM mode 1 (0110)

  TIM3->CCER |= TIM_CCER_CC3E;

  TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}

// GATE timer
static void TIM2Init(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

  TIM2->ARR = BOARD_PCLK1 / TIMER_EVENT_FREQUENCY - 1;
  TIM2->DIER = TIM_DIER_UIE;

  TIM2->SR = 0;

  NVIC_Init(TIM2_IRQn, TIMER_INTERRUPT_PRIORITY, 0, ENABLE);
}

// HSE = 8MHz
static void ClockInit(void)
{
  //RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;

  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  // voltage scaling range 1
//  while (PWR->CR1 != 0x200)
//    ;
  // boost mode off
//  while (PWR->CR5 != 0x100)
//    ;
  //The regulator is ready in the selected voltage range
//  while (PWR->SR2 & (PWR_SR2_REGLPF | PWR_SR2_VOSF))
//    ;

  // 4 wait states
  unsigned int temp = FLASH->ACR & ~FLASH_ACR_LATENCY;
  FLASH->ACR = temp | 4;

  // HSE clock selected as PLL clock entry
  // PLLM = 1
  // PLLN = 36
  // PLLR = 2
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE | (36 << RCC_PLLCFGR_PLLN_Pos) | RCC_PLLCFGR_PLLREN;

  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

void SystemInit(void)
{
  ClockInit();

  /* 144MHz / 8 => 18M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  //enable the GPIO clock for port GPIOA
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

  // enable comparator clock
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  PWR->CR3 |= PWR_CR3_UCPD_DBDIS;

  GPIOInit();
  USART2Init();
  DACSInit();
  ComparatorsInit();
  TIM1Init();
  TIM2Init();
  TIM3Init();
  TIM8Init();
}

void _init(void)
{
}

void puts_(const char *s)
{
  usart_send(USART2, s, strlen(s));
}
