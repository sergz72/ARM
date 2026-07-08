#include "board.h"
#include <dl_gpio.h>
#include <m0p/dl_sysctl.h>
#include <dl_uart_main.h>
#include <dl_timerg.h>
#include <dl_timera.h>
#include <dl_vref.h>
#include <dl_comp.h>

static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
  .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
  .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_8
};

static const DL_UART_Main_Config gUART_0Config = {
  .mode        = DL_UART_MAIN_MODE_NORMAL,
  .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
  .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
  .parity      = DL_UART_MAIN_PARITY_NONE,
  .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
  .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

// PLL VCO output = 160 MHz
// CLK2x frequcncy = 80 MHz
static const DL_SYSCTL_SYSPLLConfig gSYSPLLConfig = {
      .inputFreq              = DL_SYSCTL_SYSPLL_INPUT_FREQ_16_32_MHZ,
      .rDivClk2x              = 0,
      .rDivClk1               = 0,
      .rDivClk0               = 0,
      .enableCLK2x            = DL_SYSCTL_SYSPLL_CLK2X_DISABLE,
      .enableCLK1             = DL_SYSCTL_SYSPLL_CLK1_DISABLE,
      .enableCLK0             = DL_SYSCTL_SYSPLL_CLK0_ENABLE,
      .sysPLLMCLK             = DL_SYSCTL_SYSPLL_MCLK_CLK0,
      .sysPLLRef              = DL_SYSCTL_SYSPLL_REF_SYSOSC,
      .qDiv                   = 9, // divide by 10
      .pDiv                   = DL_SYSCTL_SYSPLL_PDIV_2
};

static const DL_TimerG_ClockConfig gPWM_0ClockConfig = {
  .clockSel = DL_TIMER_CLOCK_BUSCLK,
  .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
  .prescale = 0U
};

static const DL_TimerG_PWMConfig gPWM_0Config = {
  .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
  .period = 1000,
  .isTimerWithFourCC = false,
  .startTimer = DL_TIMER_START
};

static const DL_TimerG_ClockConfig gPERIODIC_TIMER_ClockConfig = {
  .clockSel = DL_TIMER_CLOCK_BUSCLK,
  .divideRatio = DL_TIMER_CLOCK_DIVIDE_8,
  .prescale = 0U
};

static const DL_Timer_TimerConfig gPERIODIC_TIMER_Config = {
  .period = CPUCLK_FREQ / 8 / TIMER_EVENT_FREQUENCY,
  .startTimer = DL_TIMER_STOP,
  .counterVal = 0,
  .genIntermInt = false,
  .timerMode = DL_TIMER_TIMER_MODE_PERIODIC
};

static const DL_TimerG_ClockConfig gCOUNTER_ClockConfig = {
  .clockSel = DL_TIMER_CLOCK_BUSCLK,
  .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
  .prescale = 0U
};

static const DL_TimerA_TimerConfig gCOUNTER_Config = {
  .period = 0xFFFF,
  .startTimer = DL_TIMER_STOP,
  .counterVal = 0,
  .genIntermInt = false,
  .timerMode = DL_TIMER_TIMER_MODE_PERIODIC_UP
};

static const DL_VREF_Config gVREFConfig = {
  .vrefEnable     = DL_VREF_ENABLE_ENABLE,
  .bufConfig      = DL_VREF_BUFCONFIG_OUTPUT_2_5V,
  .shModeEnable   = DL_VREF_SHMODE_DISABLE,
  .holdCycleCount = DL_VREF_HOLD_MIN,
  .shCycleCount   = DL_VREF_SH_MIN,
};

/* COMP high Initialization */
static const DL_COMP_Config gCOMP_HIGH_Config = {
  .channelEnable = DL_COMP_ENABLE_CHANNEL_POS,
  .mode          = DL_COMP_MODE_FAST,
  .negChannel    = DL_COMP_IMSEL_CHANNEL_0,
  .posChannel    = DL_COMP_IPSEL_CHANNEL_3,
  .hysteresis    = DL_COMP_HYSTERESIS_30,
  .polarity      = DL_COMP_POLARITY_NON_INV
};

/* COMP low Initialization */
static const DL_COMP_Config gCOMP_LOW_Config = {
  .channelEnable = DL_COMP_ENABLE_CHANNEL_POS,
  .mode          = DL_COMP_MODE_FAST,
  .negChannel    = DL_COMP_IMSEL_CHANNEL_0,
  .posChannel    = DL_COMP_IPSEL_CHANNEL_3,
  .hysteresis    = DL_COMP_HYSTERESIS_30,
  .polarity      = DL_COMP_POLARITY_INV
};

static const DL_COMP_RefVoltageConfig gCOMPVRefConfig = {
  .mode           = DL_COMP_REF_MODE_STATIC,
  .source         = DL_COMP_REF_SOURCE_INT_VREF_DAC,
  .terminalSelect = DL_COMP_REF_TERMINAL_SELECT_NEG,
  .controlSelect  = DL_COMP_DAC_CONTROL_SW,
  .inputSelect    = DL_COMP_DAC_INPUT_DACCODE0
};

static void GPIOInit(void)
{
  DL_GPIO_initDigitalOutput(LED_IOMUX);
  DL_GPIO_clearPins(LED_GPIO, LED_PIN);
  DL_GPIO_enableOutput(LED_GPIO, LED_PIN);
}

static void UARTInit(void)
{
  DL_GPIO_initPeripheralOutputFunction(UART_IOMUX_TX, UART_IOMUX_TX_FUNC);
  DL_GPIO_initPeripheralInputFunction(UART_IOMUX_RX, UART_IOMUX_RX_FUNC);

  DL_UART_Main_setClockConfig(UART_INSTANCE, (DL_UART_Main_ClockConfig *) &gUART_0ClockConfig);

  DL_UART_Main_init(UART_INSTANCE, (DL_UART_Main_Config *) &gUART_0Config);

  /*
   * Configure baud rate by setting oversampling and baud rate divisors.
   */
  DL_UART_Main_setOversampling(UART_INSTANCE, DL_UART_OVERSAMPLING_RATE_16X);
  DL_UART_Main_setBaudRateDivisor(UART_INSTANCE, UART_IBRD_5_MHZ_115200_BAUD, UART_FBRD_5_MHZ_115200_BAUD);

  /* Configure Interrupts */
  DL_UART_Main_enableInterrupt(UART_INSTANCE, DL_UART_MAIN_INTERRUPT_RX);

  DL_UART_Main_enable(UART0);

  NVIC_ClearPendingIRQ(UART_IRQN);
  NVIC_EnableIRQ(UART_IRQN);
}

static void InitPower(void)
{
  DL_GPIO_reset(GPIOA);
  DL_UART_Main_reset(UART_INSTANCE);
  DL_TimerG_reset(PWM_INSTANCE);
  DL_TimerG_reset(TIMG12);
  DL_TimerA_reset(COUNTER_LOW_INSTANCE);
  DL_TimerA_reset(COUNTER_HIGH_INSTANCE);
  DL_VREF_reset(VREF);
  DL_COMP_reset(COMP_LOW_INSTANCE);
  DL_COMP_reset(COMP_HIGH_INSTANCE);

  DL_GPIO_enablePower(GPIOA);
  DL_UART_Main_enablePower(UART_INSTANCE);
  DL_TimerG_enablePower(PWM_INSTANCE);
  DL_TimerG_enablePower(TIMG12);
  DL_TimerA_enablePower(COUNTER_LOW_INSTANCE);
  DL_TimerA_enablePower(COUNTER_HIGH_INSTANCE);
  DL_VREF_enablePower(VREF);
  DL_COMP_enablePower(COMP_LOW_INSTANCE);
  DL_COMP_enablePower(COMP_HIGH_INSTANCE);
  delay_cycles(POWER_STARTUP_DELAY);
}

static void CLK_init(void)
{
  while ((DL_SYSCTL_getClockStatus() & (DL_SYSCTL_CLK_STATUS_SYSPLL_GOOD
               | DL_SYSCTL_CLK_STATUS_HSCLK_GOOD
               | DL_SYSCTL_CLK_STATUS_LFOSC_GOOD))
             != (DL_SYSCTL_CLK_STATUS_SYSPLL_GOOD
               | DL_SYSCTL_CLK_STATUS_HSCLK_GOOD
               | DL_SYSCTL_CLK_STATUS_LFOSC_GOOD))
  {
    /* Ensure that clocks are in default POR configuration before initialization.
    * Additionally once LFXT is enabled, the internal LFOSC is disabled, and cannot
    * be re-enabled other than by executing a BOOTRST. */
  }
}

static void SYSCTLInit(void)
{
  //Low Power Mode is configured to be STANDBY0
  DL_SYSCTL_setPowerPolicyRUN0SLEEP0();
  DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_3);

  DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
  /* Set default configuration */
  DL_SYSCTL_disableHFXT();
  // CPU clock = 80 MHz = 2 wait states
  DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_2);

  DL_SYSCTL_configSYSPLL((DL_SYSCTL_SYSPLLConfig *) &gSYSPLLConfig);
  DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_2);
  CLK_init();
  DL_SYSCTL_setMCLKSource(SYSOSC, HSCLK, DL_SYSCTL_HSCLK_SOURCE_SYSPLL);

  /*DL_GPIO_initPeripheralOutputFunctionFeatures(CLKOUT_IOMUX,
      CLKOUT_IOMUX_FUNC, DL_GPIO_INVERSION_DISABLE,
      DL_GPIO_RESISTOR_NONE, DL_GPIO_DRIVE_STRENGTH_HIGH,
      DL_GPIO_HIZ_DISABLE);
  DL_GPIO_enableOutput(CLKOUT_PORT, CLKOUT_PIN);
  DL_SYSCTL_enableExternalClock(DL_SYSCTL_CLK_OUT_SOURCE_ULPCLK,
              DL_SYSCTL_CLK_OUT_DIVIDE_DISABLE);*/
}

static void PWMInit(void)
{
  DL_GPIO_initPeripheralOutputFunction(PWM_IOMUX,PWM_IOMUX_FUNC);
  DL_GPIO_enableOutput(PWM_PORT, PWM_PIN);

  DL_TimerG_setClockConfig(PWM_INSTANCE, (DL_TimerG_ClockConfig *) &gPWM_0ClockConfig);

  DL_TimerG_initPWMMode(PWM_INSTANCE, (DL_TimerG_PWMConfig *) &gPWM_0Config);

  // Set Counter control to the smallest CC index being used
  DL_TimerG_setCounterControl(PWM_INSTANCE,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

  DL_TimerG_setCaptureCompareOutCtl(PWM_INSTANCE, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
              DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
              DL_TIMERG_CAPTURE_COMPARE_0_INDEX);

  DL_TimerG_setCaptCompUpdateMethod(PWM_INSTANCE, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERG_CAPTURE_COMPARE_0_INDEX);
  DL_TimerG_setCaptureCompareValue(PWM_INSTANCE, 500, DL_TIMER_CC_0_INDEX);

  DL_TimerG_enableClock(PWM_INSTANCE);

  DL_TimerG_setCCPDirection(PWM_INSTANCE, DL_TIMER_CC0_OUTPUT );
}

static void VREFInit(void) {
  DL_VREF_configReference(VREF, (DL_VREF_Config *) &gVREFConfig);
}

static void COMPInit(void)
{
  DL_COMP_init(COMP_LOW_INSTANCE, (DL_COMP_Config *) &gCOMP_LOW_Config);
  DL_COMP_refVoltageInit(COMP_LOW_INSTANCE, (DL_COMP_RefVoltageConfig *) &gCOMPVRefConfig);
  DL_COMP_setDACCode0(COMP_LOW_INSTANCE, COMP_LOW_DACCODE0);
  DL_COMP_enableEvent(COMP_LOW_INSTANCE, DL_COMP_EVENT_OUTPUT_READY);
  DL_COMP_setPublisherChanID(COMP_LOW_INSTANCE, COMP_LOW_INSTANCE_PUB_CH);
  DL_COMP_enable(COMP_LOW_INSTANCE);

  DL_COMP_init(COMP_HIGH_INSTANCE, (DL_COMP_Config *) &gCOMP_HIGH_Config);
  DL_COMP_refVoltageInit(COMP_HIGH_INSTANCE, (DL_COMP_RefVoltageConfig *) &gCOMPVRefConfig);
  DL_COMP_setDACCode0(COMP_HIGH_INSTANCE, COMP_HIGH_DACCODE0);
  DL_COMP_enableEvent(COMP_HIGH_INSTANCE, DL_COMP_EVENT_OUTPUT_READY);
  DL_COMP_setPublisherChanID(COMP_HIGH_INSTANCE, COMP_HIGH_INSTANCE_PUB_CH);
  DL_COMP_enable(COMP_HIGH_INSTANCE);

  DL_GPIO_initPeripheralOutputFunction(
       COMP_LOW_IOMUX_OUT, COMP_LOW_IOMUX_OUT_FUNC);
  DL_GPIO_initPeripheralOutputFunction(
       COMP_HIGH_IOMUX_OUT, COMP_HIGH_IOMUX_OUT_FUNC);
}

static void PERIODIC_TIMER_Init()
{
  DL_TimerG_setClockConfig(TIMG12, (DL_TimerG_ClockConfig *) &gPERIODIC_TIMER_ClockConfig);

  DL_TimerG_initTimerMode(TIMG12, (DL_TimerG_TimerConfig *) &gPERIODIC_TIMER_Config);

  DL_TimerG_enableInterrupt(TIMG12 , DL_TIMERG_INTERRUPT_ZERO_EVENT);
  DL_TimerG_enableClock(TIMG12);

  NVIC_ClearPendingIRQ(TIMG12_INT_IRQn);
  NVIC_EnableIRQ(TIMG12_INT_IRQn);
}

static void COUNTER_Init(GPTIMER_Regs *gptimer, IRQn_Type IRQn, uint32_t fault_source)
{
  DL_TimerA_setClockConfig(gptimer, (DL_TimerA_ClockConfig *) &gCOUNTER_ClockConfig);

  DL_TimerA_initTimerMode(gptimer, (DL_TimerA_TimerConfig *) &gCOUNTER_Config);

  DL_TimerA_enableInterrupt(gptimer, DL_TIMERA_INTERRUPT_ZERO_EVENT);
  DL_TimerA_enableClock(gptimer);

  DL_TimerA_setFaultSourceConfig(gptimer, fault_source);
  DL_TimerA_setFaultConfig(gptimer, DL_TIMERA_FAULT_CONFIG_TFIM_DISABLED
               | DL_TIMERA_FAULT_CONFIG_FL_NO_LATCH
               | DL_TIMERA_FAULT_CONFIG_FI_DEPENDENT
               | DL_TIMERA_FAULT_CONFIG_FIEN_DISABLED);
  DL_TimerA_setFaultInputFilterConfig(gptimer,
      DL_TIMERA_FAULT_FILTER_BYPASS,
      DL_TIMERA_FAULT_FILTER_CPV_CONSEC_PER,
      DL_TIMERA_FAULT_FILTER_FP_PER_3);
  DL_TimerA_configFaultCounter(gptimer,
      DL_TIMERA_FAULT_ENTRY_CTR_SUSP_COUNT, DL_TIMERA_FAULT_EXIT_CTR_RESUME);
  DL_TimerA_enableFaultInput(gptimer);
  DL_TimerA_disableClockFaultDetection(gptimer);

  NVIC_ClearPendingIRQ(IRQn);
  NVIC_EnableIRQ(IRQn);
}

static void COUNTER_HIGH_Init(void)
{
}

void SystemInit(void)
{
  SYSCTLInit();
  InitPower();
  GPIOInit();
  UARTInit();
  PWMInit();
  VREFInit();
  COMPInit();
  PERIODIC_TIMER_Init();
  COUNTER_Init(COUNTER_LOW_INSTANCE, COUNTER_LOW_IRQN, DL_TIMERA_FAULT_SOURCE_COMP0_SENSE_HIGH);
  COUNTER_Init(COUNTER_HIGH_INSTANCE, COUNTER_HIGH_IRQN, DL_TIMERA_FAULT_SOURCE_COMP1_SENSE_HIGH);
}

unsigned int mv_to_8(unsigned int mv)
{
  if (mv >= DAC_REFERENCE_VOLTAGE)
    return 255;
  return (mv * 255) / DAC_REFERENCE_VOLTAGE;
}

void __attribute__((section(".ramfunc"))) stop_counters(void)
{
  //todo
  DL_TimerG_stopCounter(TIMG12);
  DL_TimerA_stopCounter(COUNTER_LOW_INSTANCE);
  DL_TimerA_stopCounter(COUNTER_HIGH_INSTANCE);
}

void __attribute__((section(".ramfunc"))) start_counters(void)
{
  //todo
  DL_TimerG_startCounter(TIMG12);
  DL_TimerA_startCounter(COUNTER_LOW_INSTANCE);
  DL_TimerA_startCounter(COUNTER_HIGH_INSTANCE);
}

void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty)
{
  unsigned int arr = CPUCLK_FREQ / frequency;
  if (arr < 2)
    arr = 2;
  if (arr > 65536)
    arr = 65536;
  if (duty == 0)
    duty = 1;
  else if (duty > 99)
    duty = 99;
  //todo
  DL_TimerG_setLoadValue(PWM_INSTANCE, arr - 1);
  DL_TimerG_setCaptureCompareValue(PWM_INSTANCE, arr * duty / 100, DL_TIMER_CC_0_INDEX);
}

void __attribute__((section(".ramfunc"))) puts_(const char *s)
{
  for (;;)
  {
    char c = *s++;
    if (!c)
      return;
    DL_UART_Main_transmitDataBlocking(UART_INSTANCE, c);
  }
}
