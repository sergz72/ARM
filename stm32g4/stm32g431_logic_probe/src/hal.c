#include "board.h"
#include <systick.h>
#include <gpio.h>
#include <nvic.h>
#include <string.h>
#include <usart.h>
#include <i2c.h>
#include <spi.h>
#include <ws2812_spi.h>

static const SPI_InitStruct spi1_init = {
  .master = 1,
  .clock_phase = 0,
  .clock_polarity = 0,
  .data_size = 8,
  .error_interrupt_enable = 0,
  .rxne_interrupt_enable = 0,
  .txe_interrupt_enable = 0,
  .lsb_first = 0,
  .rxonly = 0,
  .ss_output_enable = 0,
  .ti_mode = 0,
  .tx_dma_enable = 1,
  .rx_dma_enable = 0,
  .software_slave_management = 1,
  .internal_slave_select = 1,
  .baud_rate = 2500000
};

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

  // Configure TIM1 in gated mode
  TIM1->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_2 |                  // Gated mode
               TIM_SMCR_TS_0 | TIM_SMCR_TS_1 | TIM_SMCR_TS_2;    // tim_etrf as trigger source

  TIM1->AF1 = TIM1_AF1_ETRSEL_0; // tim_etr1(comp1_out) as tim_etrf

  TIM1->DIER = TIM_DIER_UIE;

  TIM1->SR = 0;
  NVIC_Init(TIM1_UP_TIM16_IRQn, COUNTERS_INTERRUPT_PRIORITY, 0, ENABLE);
}

static void TIM8Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;

  // Configure TIM8 in gated mode
  TIM8->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_2 |                  // Gated mode
               TIM_SMCR_TS_0 | TIM_SMCR_TS_1 | TIM_SMCR_TS_2;    // tim_etrf as trigger source

  TIM8->AF1 = TIM1_AF1_ETRSEL_0 | TIM1_AF1_ETRSEL_1; // tim_etr3(comp3_out) as tim_etrf

  TIM8->DIER = TIM_DIER_UIE;

  TIM8->SR = 0;
  NVIC_Init(TIM8_UP_IRQn, COUNTERS_INTERRUPT_PRIORITY, 0, ENABLE);
}

static void TIM4Init(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;

  // Configure TIM in external clock mode
  TIM4->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2 |
               TIM_SMCR_TS_0 | TIM_SMCR_TS_1 | TIM_SMCR_TS_2 | TIM_SMCR_ETF_0;    // tim_etrf as trigger source

  TIM4->AF1 = TIM1_AF1_ETRSEL_0; // tim_etr1(comp1_out) as tim_etrf

  TIM4->DIER = TIM_DIER_UIE;

  TIM4->SR = 0;
  NVIC_Init(TIM4_IRQn, COUNTERS_INTERRUPT_PRIORITY, 0, ENABLE);
}

static void TIM3Init(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;

  // Configure TIM in external clock mode
  TIM3->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2 |
                TIM_SMCR_TS_0 | TIM_SMCR_TS_1 | TIM_SMCR_TS_2;    // tim_etrf as trigger source

  TIM3->AF1 = TIM1_AF1_ETRSEL_0 | TIM1_AF1_ETRSEL_1; // tim_etr3(comp3_out) as tim_etrf

  TIM3->DIER = TIM_DIER_UIE;

  TIM3->SR = 0;
  NVIC_Init(TIM3_IRQn, COUNTERS_INTERRUPT_PRIORITY, 0, ENABLE);
}

void __attribute__((section(".RamFunc"))) stop_counters(void)
{
  TIM4->CR1 &= ~TIM_CR1_CEN;
  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM2->CR1 &= ~TIM_CR1_CEN;
  TIM1->CR1 &= ~TIM_CR1_CEN;
  TIM8->CR1 &= ~TIM_CR1_CEN;
}

void __attribute__((section(".RamFunc"))) start_counters(void)
{
  TIM1->CNT = 0;
  TIM8->CNT = 0;
  TIM4->CNT = 0;
  TIM2->CNT = 0;
  TIM3->CNT = 0;
  TIM1->CR1 |= TIM_CR1_CEN;
  TIM8->CR1 |= TIM_CR1_CEN;
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM4->CR1 |= TIM_CR1_CEN;
  TIM3->CR1 |= TIM_CR1_CEN;
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
  TIM17->ARR = arr - 1;
  TIM17->CCR1 = arr * duty / 100;
}

// PWM timer
static void TIM17Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;

  GPIO_Init(GPIOB,
            GPIO_Pin_9,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_1);

  pwm_set_frequency_and_duty(10000, 50);

  TIM17->CCMR1 = (6 << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;  // PWM mode 1 (0110)

  TIM17->CCER |= TIM_CCER_CC1E;

  TIM17->BDTR = TIM_BDTR_MOE;

  TIM17->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
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

/*
 * PA7=SPI1_MOSI
 */
static void SPI1Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  GPIO_Init(GPIOA,
            GPIO_Pin_7,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_5);

  SPI_Init(SPI1, &spi1_init);
  SPI_Enable(SPI1);
}

/*
 * PA8=SDA
 * PC4=SCL
 */
static void I2C2Init(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_I2C2EN;

  GPIO_Init(GPIOA,
            GPIO_Pin_8,
            GPIO_Mode_AF,
            GPIO_Speed_Low,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );
  GPIO_Init(GPIOC,
            GPIO_Pin_4,
            GPIO_Mode_AF,
            GPIO_Speed_Low,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_4);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_4);

  I2C_Master_Init(I2C2, I2C_TIMINGS); // calculated by stm32cubemx
  I2C_Enable(I2C2);
}

static void DMAInit(void)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMAMUX1EN;

  // DMA channel 1 = from mem to SPI
  DMA1_Channel1->CCR = DMA_CCR_MINC | // memory increment mode
                       DMA_CCR_PL | // very high priority level
                       DMA_CCR_DIR; // read from memory
  DMA1_Channel1->CPAR = (unsigned int)&SPI1->DR;

  // 11 = SPI1_TX
  DMAMUX1_Channel0->CCR = 11;

  // DMA channel 2 = from mem to I2C2
  DMA1_Channel2->CCR = DMA_CCR_MINC | // memory increment mode
                       DMA_CCR_PL | // very high priority level
                       DMA_CCR_DIR; // read from memory
  DMA1_Channel2->CPAR = (unsigned int)&I2C2->TXDR;

  // 19 = I2C2_TX
  DMAMUX1_Channel2->CCR = 19;
}


// HSE = 8MHz
static void ClockInit(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;

  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  // voltage scaling range 1
//  while (PWR->CR1 != 0x200)
//    ;
#ifdef BOOST_MODE
  // boost mode on
  PWR->CR5 = 0;
#endif

  //The regulator is ready in the selected voltage range
  while (PWR->SR2 & (PWR_SR2_REGLPF | PWR_SR2_VOSF))
    ;

  // 4 wait states
  unsigned int temp = FLASH->ACR & ~FLASH_ACR_LATENCY;
  FLASH->ACR = temp | 4 | FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN;

  // HSE clock selected as PLL clock entry
  // PLLM = 1
  // PLLN = 36
  // PLLR = 2
  // PLLQ = 6
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE | (PLLN << RCC_PLLCFGR_PLLN_Pos) | RCC_PLLCFGR_PLLREN | RCC_PLLCFGR_PLLQEN
                 | RCC_PLLCFGR_PLLQ_1;

  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

static void USB_Init(void)
{
  RCC->CCIPR |= RCC_CCIPR_CLK48SEL_1; // pll1_q_ck selected as usb kernel clock
  RCC->APB1ENR1 |= RCC_APB1ENR1_USBEN;

  NVIC_Init(USB_LP_IRQn, USART_INTERRUPT_PRIORITY, 3, ENABLE);
}

void SystemInit(void)
{
  ClockInit();

  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

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
  DACSInit();
  ComparatorsInit();
  TIM1Init();
  TIM2Init();
  TIM17Init();
  TIM4Init();
  TIM3Init();
  TIM8Init();
  SPI1Init();
  I2C2Init();
  DMAInit();
  USB_Init();
}

void _init(void)
{
}

int __attribute__((section(".RamFunc"))) SSD1306_I2C_Write(int num_bytes, unsigned char control_byte, unsigned char *buffer)
{
  static unsigned char i2c_buffer[256];

  i2c_buffer[0] = control_byte;
  memcpy(i2c_buffer + 1, buffer, num_bytes);
  return I2C_Write(I2C2, SSD1306_I2C_ADDRESS, i2c_buffer, num_bytes + 1, I2C_TIMEOUT);
}

void __attribute__((section(".RamFunc"))) ws2812_spi_send(int channel, const unsigned char *data, unsigned int count)
{
  //while (count--)
    //SPI_Send8(SPI1, *data++, SPI_TIMEOUT);

  DMA1_Channel1->CCR &= ~DMA_CCR_EN;
  DMA1_Channel1->CMAR = (unsigned int)data;
  DMA1_Channel1->CNDTR = count;
  DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void __attribute__((section(".RamFunc"))) set_h_voltage(unsigned int value)
{
  DAC3->DHR12R1 = mv_to_12(value);
}

void __attribute__((section(".RamFunc"))) set_l_voltage(unsigned int value)
{
  DAC1->DHR12R1 = mv_to_12(value);
}

void set_dac_voltage(unsigned int value)
{
  DAC1->DHR12R2 = mv_to_12(value);
}

unsigned int get_dac_voltage(void)
{
  return mv_from_12(DAC1->DHR12R2);
}

unsigned int get_l_voltage(void)
{
  return mv_from_12(DAC1->DHR12R1);
}

unsigned int get_h_voltage(void)
{
  return mv_from_12(DAC3->DHR12R1);
}