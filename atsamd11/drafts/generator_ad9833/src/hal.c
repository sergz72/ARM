#include "hal.h"

#define ADC_VREF 2230 //mv

static unsigned char *rxbuf_p, *txbuf_p;

static inline void pointers_reset(void *rxaddress, const void *txaddress)
{
  rxbuf_p = (unsigned char*)rxaddress;
  txbuf_p = (unsigned char*)txaddress;
}

void __attribute__((used)) RTC_Handler(void)
{
  RTC_REGS->MODE0.RTC_INTFLAG = RTC_MODE0_INTFLAG_Msk;
  timer_interrupt = 1;
}

void __attribute__((used)) EIC_Handler(void)
{
  pointers_reset(rxbuf, txbufs[rxbuf[0] & 7]);
  //SERCOM0_REGS->SPIS.SERCOM_DATA = *txbuf_p++;
  unsigned char dummy = SERCOM0_REGS->SPIS.SERCOM_DATA;
  (void)dummy;
  command_ready = 1;
  EIC_REGS->EIC_INTFLAG = 0xFF; // clear flags
}

void __attribute__((used)) SERCOM0_Handler(void)
{
  unsigned char flags = SERCOM0_REGS->SPIS.SERCOM_INTFLAG;
  if (flags & SERCOM_SPIS_INTFLAG_DRE(1))
    SERCOM0_REGS->SPIS.SERCOM_DATA = *txbuf_p++;
  if (flags & SERCOM_SPIS_INTFLAG_RXC(1))
    *rxbuf_p++ = SERCOM0_REGS->SPIS.SERCOM_DATA;
}

/*
 * PAD2 = MISO = PA04(14)
 * PAD3 = SCK  = PA05(1)
 * PAD1 = NSS  = PA15(5)
 * PAD0 = MOSI = PA14(4)
 */

static void spi_slave_init(void)
{
  //MISO - out
  PORT_REGS->GROUP[0].PORT_PINCFG[4] = 1; // PMUXEN
  // SCK -in with pulldown
  PORT_REGS->GROUP[0].PORT_PINCFG[5] = 7; // PMUXEN | INEN | PULLEN
  PORT_REGS->GROUP[0].PORT_PMUX[2] = 0x22;
  // NSS -in with pullup
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA15;
  PORT_REGS->GROUP[0].PORT_PINCFG[15] = 7; // PMUXEN | INEN | PULLEN
  // MOSI -in with pulldown
  PORT_REGS->GROUP[0].PORT_PINCFG[14] = 7; // PMUXEN | INEN | PULLEN
  PORT_REGS->GROUP[0].PORT_PMUX[7] = 0x22;

  /* Selection of the Character Size and Receiver Enable, Hardware _SS control is enabled */
  SERCOM0_REGS->SPIS.SERCOM_CTRLB = SERCOM_SPIS_CTRLB_CHSIZE_8_BIT | SERCOM_SPIS_CTRLB_RXEN(1);

  /* Wait for synchronization */
  while((SERCOM0_REGS->SPIS.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  /* Configure Data Out Pin, Master Mode,
   * Data In and Pin Out,Data Order and Standby mode if configured
   * and Selection of the Clock Phase and Polarity and Enable the SPI Module
   */
  SERCOM0_REGS->SPIS.SERCOM_CTRLA = SERCOM_SPIS_CTRLA_MODE_SPI_SLAVE | SERCOM_SPIS_CTRLA_DOPO_PAD1 |
    SERCOM_SPIS_CTRLA_DIPO_PAD0 | SERCOM_SPIS_CTRLA_CPOL_IDLE_LOW | SERCOM_SPIS_CTRLA_CPHA_LEADING_EDGE |
      SERCOM_SPIS_CTRLA_DORD_MSB | SERCOM_SPIS_CTRLA_ENABLE(1) ;


  /* Wait for synchronization */
  while((SERCOM0_REGS->SPIS.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  SERCOM0_REGS->SPIS.SERCOM_INTENSET = SERCOM_SPIS_INTENSET_ERROR(1) | SERCOM_SPIS_INTENSET_RXC(1) | SERCOM_SPIS_INTENSET_DRE(1);

  NVIC_SetPriority(SERCOM0_IRQn, 0);
  NVIC_EnableIRQ(SERCOM0_IRQn);
}

/*
 * PAD2 = MOSI = PA08(2)
 * PAD3 = SCK  = PA09(3)
 * NSS = PA28(6)
 */

static void spi_master_init(void)
{
#ifdef EXTERNAL_RESET_DISABLE
  PM_REGS->PM_EXTCTRL = 1; //External Reset Disable
#endif
  PORT_REGS->GROUP[0].PORT_OUTSET = SPI_MASTER_NSS_PIN;
  PORT_REGS->GROUP[0].PORT_DIRSET = SPI_MASTER_NSS_PIN;

  /* Selection of the Character Size and Receiver Disable */
  SERCOM1_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;

  /* Wait for synchronization */
  while((SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  /* Selection of the Baud Value */
  SERCOM1_REGS->SPIM.SERCOM_BAUD = (uint8_t)SERCOM_SPIM_BAUD_BAUD(SERCOM1_SPIM_BAUD_VALUE);

  /* Configure Data Out Pin, Master Mode,
   * Data In and Pin Out,Data Order and Standby mode if configured
   * and Selection of the Clock Phase and Polarity and Enable the SPI Module
   */
  SERCOM1_REGS->SPIM.SERCOM_CTRLA = SERCOM_SPIM_CTRLA_MODE_SPI_MASTER | SERCOM_SPIM_CTRLA_DOPO_PAD1 |
    SERCOM_SPIM_CTRLA_DIPO_PAD0 | SERCOM_SPIM_CTRLA_CPOL_IDLE_LOW | SERCOM_SPIM_CTRLA_CPHA_LEADING_EDGE |
      SERCOM_SPIM_CTRLA_DORD_MSB | SERCOM_SPIM_CTRLA_ENABLE(1) ;


  /* Wait for synchronization */
  while((SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }
}

/*
 *PA24(9) = EXTINT[4] (connected to NSS = PA15(5))
 */

static void exti_init(void)
{
  // in with pullup
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA24;
  PORT_REGS->GROUP[0].PORT_PINCFG[24] = 7; // PMUXEN | INEN | PULLEN
  PORT_REGS->GROUP[0].PORT_PMUX[12] = 0; // EIC

  /* Interrupt sense type and filter control for EXTINT channels 0 to 7 */
  EIC_REGS->EIC_CONFIG[0] = EIC_CONFIG_SENSE0_NONE  |
                            EIC_CONFIG_SENSE1_NONE  |
                            EIC_CONFIG_SENSE2_NONE  |
                            EIC_CONFIG_SENSE3_NONE  |
                            EIC_CONFIG_SENSE4_RISE  |
                            EIC_CONFIG_SENSE5_NONE  |
                            EIC_CONFIG_SENSE6_NONE  |
                            EIC_CONFIG_SENSE7_NONE ;

  /* Event Control Output enable */
  EIC_REGS->EIC_INTENSET = 0x10;

  /* Enable the EIC */
  EIC_REGS->EIC_CTRL = EIC_CTRL_ENABLE(1);
  while((EIC_REGS->EIC_STATUS & EIC_STATUS_SYNCBUSY_Msk) == EIC_STATUS_SYNCBUSY_Msk)
  {
    /* Wait for sync */
  }

  NVIC_SetPriority(EIC_IRQn, 1);
  NVIC_EnableIRQ(EIC_IRQn);
}

static void rtc_init(void)
{
  /* Writing to CTRL register will trigger write-synchronization */
  RTC_REGS->MODE0.RTC_CTRL = RTC_MODE0_CTRL_MODE(0U) | RTC_MODE0_CTRL_PRESCALER(0x0U) | RTC_MODE0_CTRL_MATCHCLR_Msk;
  while((RTC_REGS->MODE0.RTC_STATUS & RTC_STATUS_SYNCBUSY_Msk) == RTC_STATUS_SYNCBUSY_Msk)
  {
    /* Wait for Write-Synchronization */
  }

  /* Writing to COMP register will trigger write-synchronization */
  RTC_REGS->MODE0.RTC_COMP = 164; // 5 ms interrupt
  while((RTC_REGS->MODE0.RTC_STATUS & RTC_STATUS_SYNCBUSY_Msk) == RTC_STATUS_SYNCBUSY_Msk)
  {
    /* Wait for Write-Synchronization */
  }

  RTC_REGS->MODE0.RTC_INTENSET = 1; // compare match

  NVIC_SetPriority(RTC_IRQn, 2);
  NVIC_EnableIRQ(RTC_IRQn);
}

/*
 * PA02(13) = AIN0
 */

#define ADC_LINEARITY0_POS  (27U)
#define ADC_LINEARITY0_Msk   ((0x1FUL << ADC_LINEARITY0_POS))

#define ADC_LINEARITY1_POS  (0U)
#define ADC_LINEARITY1_Msk   ((0x7U << ADC_LINEARITY1_POS))

#define ADC_BIASCAL_POS  (3U)
#define ADC_BIASCAL_Msk   ((0x7U << ADC_BIASCAL_POS))

static void adc_init(void)
{
  uint32_t adc_linearity0 = (((*(uint32_t*)OTP4_ADDR) & ADC_LINEARITY0_Msk) >> ADC_LINEARITY0_POS);
  uint32_t adc_linearity1 = (((*(uint32_t*)(OTP4_ADDR + 4U)) & ADC_LINEARITY1_Msk) >> ADC_LINEARITY1_POS);

  /* Write linearity calibration and bias calibration */
  ADC_REGS->ADC_CALIB = (uint16_t)((ADC_CALIB_LINEARITY_CAL(adc_linearity0 | (adc_linearity1 << 5U))) \
      | ADC_CALIB_BIAS_CAL((((*(uint32_t*)(OTP4_ADDR + 4U)) & ADC_BIASCAL_Msk) >> ADC_BIASCAL_POS)));

  /* Sampling length */
  ADC_REGS->ADC_SAMPCTRL = ADC_SAMPCTRL_SAMPLEN(0x3F);

  /* reference is 1/1.48 VDDANA ~2.23v */
  ADC_REGS->ADC_REFCTRL = ADC_REFCTRL_REFSEL_INTVCC0;

  /* positive and negative input pins */
  ADC_REGS->ADC_INPUTCTRL = (uint32_t) ADC_INPUTCTRL_MUXNEG_PIN0 | (uint32_t) ADC_INPUTCTRL_MUXNEG_GND \
      | ADC_INPUTCTRL_INPUTSCAN(0U) | ADC_INPUTCTRL_INPUTOFFSET(0U) | ADC_INPUTCTRL_GAIN_1X;
  while((ADC_REGS->ADC_STATUS & ADC_STATUS_SYNCBUSY_Msk)!= 0U)
  {
    /* Wait for Synchronization */
  }

  /* Prescaler, Resolution & Operation Mode */
  ADC_REGS->ADC_CTRLB = ADC_CTRLB_PRESCALER_DIV32 | ADC_CTRLB_RESSEL_12BIT ;
  while((ADC_REGS->ADC_STATUS & ADC_STATUS_SYNCBUSY_Msk)!= 0U)
  {
    /* Wait for Synchronization */
  }

  /* Clear all interrupt flags */
  ADC_REGS->ADC_INTFLAG = ADC_INTFLAG_Msk;

  while((ADC_REGS->ADC_STATUS & ADC_STATUS_SYNCBUSY_Msk) != 0U)
  {
    /* Wait for Synchronization */
  }

  ADC_REGS->ADC_CTRLA |= ADC_CTRLA_ENABLE_Msk;
  while((ADC_REGS->ADC_STATUS & ADC_STATUS_SYNCBUSY_Msk) != 0U)
  {
    /* Wait for Synchronization */
  }
}

static void gclk0_init(void)
{
  // source=DFLL48M output
  GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_SRC(7) | GCLK_GENCTRL_GENEN_Msk | GCLK_GENCTRL_ID(0);
  while((GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk) == GCLK_STATUS_SYNCBUSY_Msk)
  {
    /* wait for the Generator 0 synchronization */
  }

  /* Selection of the Generator and write Lock for SERCOM0_CORE */
  GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_SERCOM0_CORE | GCLK_CLKCTRL_GEN(0x0)  | GCLK_CLKCTRL_CLKEN_Msk;
  /* Selection of the Generator and write Lock for SERCOM1_CORE */
  GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_SERCOM1_CORE | GCLK_CLKCTRL_GEN(0x0)  | GCLK_CLKCTRL_CLKEN_Msk;
  /* Selection of the Generator and write Lock for ADC */
  GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_ADC | GCLK_CLKCTRL_GEN(0x0)  | GCLK_CLKCTRL_CLKEN_Msk;
  /* Selection of the Generator and write Lock for EIC */
  GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_EIC | GCLK_CLKCTRL_GEN(0x0)  | GCLK_CLKCTRL_CLKEN_Msk;
}

static void gclk1_init(void)
{
  GCLK_REGS->GCLK_GENCTRL = GCLK_GENCTRL_SRC(4U) | GCLK_GENCTRL_GENEN_Msk | GCLK_GENCTRL_ID(1U);

  //GCLK_REGS->GCLK_GENDIV = GCLK_GENDIV_DIV(32U) | GCLK_GENDIV_ID(1U);

  while((GCLK_REGS->GCLK_STATUS & GCLK_STATUS_SYNCBUSY_Msk) == GCLK_STATUS_SYNCBUSY_Msk)
  {
    /* wait for the Generator 1 synchronization */
  }

  /* Selection of the Generator and write Lock for RTC */
  GCLK_REGS->GCLK_CLKCTRL = GCLK_CLKCTRL_ID_RTC | GCLK_CLKCTRL_GEN(0x1U)  | GCLK_CLKCTRL_CLKEN_Msk;
}

static void clock_init(void)
{
  SYSCTRL_REGS->SYSCTRL_XOSC = 0;
  SYSCTRL_REGS->SYSCTRL_XOSC32K = 0;

  /* Configure 32K RC oscillator */
  SYSCTRL_REGS->SYSCTRL_OSC32K = (SYSCTRL_REGS->SYSCTRL_OSC32K & 0xFFFFFF00) | SYSCTRL_OSC32K_ENABLE_Msk | SYSCTRL_OSC32K_EN32K_Msk;
  while(!((SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_OSC32KRDY_Msk) == SYSCTRL_PCLKSR_OSC32KRDY_Msk))
  {
    /* Waiting for the OSC32K Ready state */
  }

  SYSCTRL_REGS->SYSCTRL_DFLLCTRL = 2; // Enable DFLL in open loop mode
  while((SYSCTRL_REGS->SYSCTRL_PCLKSR & SYSCTRL_PCLKSR_DFLLRDY_Msk) != SYSCTRL_PCLKSR_DFLLRDY_Msk)
  {
    /* Waiting for the Ready state */
  }

  gclk0_init();
  gclk1_init();

  // enable adc, sercom0, sercom1 clocks
  PM_REGS->PM_APBCMASK = 0x10C;

  SYSCTRL_REGS->SYSCTRL_OSC8M = 0;
}

/*
 * PA25(10) = LED1
 */

static void ports_init(void)
{
  PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA25;
  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA25;
}

void SysInit(void *rxaddress, const void *txaddress)
{
  pointers_reset(rxaddress, txaddress);
  // 1 wait state - 2.7V to 3.63V up to 67 MHz clock
  NVMCTRL_REGS->NVMCTRL_CTRLB = NVMCTRL_CTRLB_RWS(1);
  clock_init();
  ports_init();
  spi_slave_init();
  spi_master_init();
  exti_init();
  rtc_init();
  adc_init();
}

static void SERCOM1_SPI_Write(const unsigned char *data, int length)
{
  PORT_REGS->GROUP[0].PORT_OUTCLR = SPI_MASTER_NSS_PIN;

  /* Flush out any unread data in SPI read buffer */
  while((SERCOM1_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk) == SERCOM_SPIM_INTFLAG_RXC_Msk)
  {
    uint32_t dummyData = SERCOM1_REGS->SPIM.SERCOM_DATA;
    (void)dummyData;
  }

  SERCOM1_REGS->SPIM.SERCOM_STATUS |= SERCOM_SPIM_STATUS_BUFOVF_Msk;

  SERCOM1_REGS->SPIM.SERCOM_INTFLAG |= (uint8_t)SERCOM_SPIM_INTFLAG_ERROR_Msk;

  while (length--)
  {
    /* Make sure DRE is empty */
    while((SERCOM1_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk) != SERCOM_SPIM_INTFLAG_DRE_Msk)
    {
      /* Do nothing */
    }
    SERCOM1_REGS->SPIM.SERCOM_DATA = *data++;
  }

  /* Make sure DRE is empty */
  while((SERCOM1_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk) != SERCOM_SPIM_INTFLAG_DRE_Msk)
  {
    /* Do nothing */
  }

  PORT_REGS->GROUP[0].PORT_OUTSET = SPI_MASTER_NSS_PIN;
}

void ad9833_write(int channel, unsigned short data)
{
  SERCOM1_SPI_Write((const unsigned char*)&data, 2);
}

unsigned short adc_get(void)
{
  /* Start conversion */
  ADC_REGS->ADC_SWTRIG |= ADC_SWTRIG_START_Msk;
  while (!(ADC_REGS->ADC_INTFLAG & ADC_INTFLAG_RESRDY_Msk))
    ;
  ADC_REGS->ADC_INTFLAG = ADC_INTFLAG_RESRDY_Msk;
  unsigned int result = ADC_REGS->ADC_RESULT;
  unsigned short mV = (unsigned short)((result * ADC_VREF) >> 12);
  return mV;
}

void timer_enable(void)
{
  /* Writing to CTRL register will trigger write-synchronization */
  RTC_REGS->MODE0.RTC_CTRL |= RTC_MODE0_CTRL_ENABLE_Msk;
  /*while((RTC_REGS->MODE0.RTC_STATUS & RTC_STATUS_SYNCBUSY_Msk) == RTC_STATUS_SYNCBUSY_Msk)
  {
    // Wait for Write-synchronization
  }*/
  __enable_irq();
}

void status_updated(void)
{
}
