#include "board.h"
#include <dl_gpio.h>
#include <m0p/dl_sysctl.h>
#include <dl_uart_main.h>
#include <dl_timerg.h>
#include <dl_timera.h>
#include <dl_comp.h>
#include <dl_adc12.h>
#include <dl_dac12.h>
#include <dl_opa.h>
#include <dl_vref.h>

static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
  .clockSel    = DL_UART_MAIN_CLOCK_MFCLK,
  .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_0Config = {
  .mode        = DL_UART_MAIN_MODE_NORMAL,
  .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
  .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
  .parity      = DL_UART_MAIN_PARITY_NONE,
  .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
  .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

static const DL_TimerA_ClockConfig gPWM_ClockConfig = {
  .clockSel = DL_TIMER_CLOCK_BUSCLK,
  .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
  .prescale = 0U
};

static const DL_TimerA_PWMConfig gPWM_Config = {
  .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
  .period = 1000,
  .isTimerWithFourCC = false,
  .startTimer = DL_TIMER_START
};

static const DL_TimerG_ClockConfig gPERIODIC_TIMER_ClockConfig = {
  .clockSel = DL_TIMER_CLOCK_LFCLK,
  .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
  .prescale = 0U
};

static const DL_Timer_TimerConfig gPERIODIC_TIMER_Config = {
  .period = LFCLK_FREQ / TIMER_EVENT_FREQUENCY,
  .startTimer = DL_TIMER_STOP,
  .counterVal = 0,
  .genIntermInt = false,
  .timerMode = DL_TIMER_TIMER_MODE_PERIODIC
};

/* COMP high Initialization */
static const DL_COMP_Config gCOMP_Config = {
  .channelEnable = DL_COMP_ENABLE_CHANNEL_POS,
  .mode          = DL_COMP_MODE_FAST,
  .negChannel    = DL_COMP_IMSEL_CHANNEL_0,
  .posChannel    = DL_COMP_IPSEL_CHANNEL_3,
  .hysteresis    = DL_COMP_HYSTERESIS_30,
  .polarity      = DL_COMP_POLARITY_NON_INV
};

static const DL_COMP_RefVoltageConfig gCOMPVRefConfig = {
  .mode           = DL_COMP_REF_MODE_STATIC,
  .source         = DL_COMP_REF_SOURCE_INT_VREF_DAC,
  .terminalSelect = DL_COMP_REF_TERMINAL_SELECT_NEG,
  .controlSelect  = DL_COMP_DAC_CONTROL_SW,
  .inputSelect    = DL_COMP_DAC_INPUT_DACCODE0
};

/* ADC12_0 Initialization */
static const DL_ADC12_ClockConfig gADC_ClockConfig = {
  .clockSel       = DL_ADC12_CLOCK_SYSOSC,
  .divideRatio    = DL_ADC12_CLOCK_DIVIDE_1,
  .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_1_TO_4
};

static const DL_VREF_ClockConfig gVREFClockConfig = {
  .clockSel = DL_VREF_CLOCK_LFCLK,
  .divideRatio = DL_VREF_CLOCK_DIVIDE_1,
};

static const DL_VREF_Config gVREFConfig = {
  .vrefEnable     = DL_VREF_ENABLE_ENABLE,
  .bufConfig      = DL_VREF_BUFCONFIG_OUTPUT_1_4V,
  .shModeEnable   = DL_VREF_SHMODE_DISABLE,
  .holdCycleCount = DL_VREF_HOLD_MIN,
  .shCycleCount   = DL_VREF_SH_MIN,
};

static const DL_DAC12_Config gDAC12Config = {
  .outputEnable              = DL_DAC12_OUTPUT_ENABLED,
  .resolution                = DL_DAC12_RESOLUTION_12BIT,
  .representation            = DL_DAC12_REPRESENTATION_BINARY,
  .voltageReferenceSource    = DL_DAC12_VREF_SOURCE_VEREFP_VSSA,
  .amplifierSetting          = DL_DAC12_AMP_ON,
  .fifoEnable                = DL_DAC12_FIFO_DISABLED,
  .fifoTriggerSource         = DL_DAC12_FIFO_TRIGGER_SAMPLETIMER,
  .dmaTriggerEnable          = DL_DAC12_DMA_TRIGGER_DISABLED,
  .dmaTriggerThreshold       = DL_DAC12_FIFO_THRESHOLD_ONE_QTR_EMPTY,
  .sampleTimeGeneratorEnable = DL_DAC12_SAMPLETIMER_DISABLE,
  .sampleRate                = DL_DAC12_SAMPLES_PER_SECOND_500
};

static const DL_OPA_Config gOPA1Config = {
  .pselChannel    = DL_OPA_PSEL_DAC_OUT,
  .nselChannel    = DL_OPA_NSEL_RTAP,
  .mselChannel    = DL_OPA_MSEL_IN1_NEG,
  .gain           = DL_OPA_GAIN_N31_P32,
  .outputPinState = DL_OPA_OUTPUT_PIN_DISABLED,
  .choppingMode   = DL_OPA_CHOPPING_MODE_DISABLE
};

static void GPIOInit(void)
{
  DL_GPIO_initDigitalOutput(LED_TIMER_IOMUX);
  DL_GPIO_clearPins(LED_TIMER_GPIO, LED_TIMER_PIN);
  DL_GPIO_enableOutput(LED_TIMER_GPIO, LED_TIMER_PIN);

  DL_GPIO_initDigitalOutput(LED_MOTION_IOMUX);
  clear_motion_led();
  DL_GPIO_enableOutput(LED_TIMER_GPIO, LED_MOTION_PIN);
}

void toggle_timer_led(void)
{
  DL_GPIO_togglePins(LED_TIMER_GPIO, LED_TIMER_PIN);
}

void set_motion_led(void)
{
  DL_GPIO_clearPins(LED_MOTION_GPIO, LED_MOTION_PIN);
}

void clear_motion_led(void)
{
  DL_GPIO_setPins(LED_MOTION_GPIO, LED_MOTION_PIN);
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
  DL_UART_Main_setBaudRateDivisor(UART_INSTANCE, UART_IBRD_4_MHZ_115200_BAUD, UART_FBRD_4_MHZ_115200_BAUD);

  /* Configure Interrupts */
  DL_UART_Main_enableInterrupt(UART_INSTANCE, DL_UART_MAIN_INTERRUPT_RX);

  DL_UART_Main_enable(UART_INSTANCE);

  NVIC_SetPriority(UART_IRQN, UART_INTERRUPT_PRIORITY);
  NVIC_ClearPendingIRQ(UART_IRQN);
  NVIC_EnableIRQ(UART_IRQN);
}

static void InitPower(void)
{
  DL_GPIO_reset(GPIOA);
  DL_UART_Main_reset(UART_INSTANCE);
  DL_TimerA_reset(PWM_INSTANCE);
  DL_TimerG_reset(PERIODIC_TIMER_INSTANCE);
  DL_ADC12_reset(ADC_INST);
  DL_VREF_reset(VREF);
  DL_DAC12_reset(DAC0);
  DL_OPA_reset(OPA_INST);

  DL_GPIO_enablePower(GPIOA);
  DL_UART_Main_enablePower(UART_INSTANCE);
  DL_TimerG_enablePower(PERIODIC_TIMER_INSTANCE);
  DL_VREF_enablePower(VREF);
  DL_ADC12_enablePower(ADC_INST);
  DL_DAC12_enablePower(DAC0);
  DL_OPA_enablePower(OPA_INST);
  delay_cycles(POWER_STARTUP_DELAY);
}

static void SYSCTLInit(void)
{
  //Low Power Mode is configured to be STANDBY0
  DL_SYSCTL_setPowerPolicyRUN0SLEEP0();
  DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_3);

  DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_4M);
  DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
  /* Set default configuration */
  DL_SYSCTL_disableHFXT();
  DL_SYSCTL_disableSYSPLL();
  DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);
  DL_SYSCTL_enableMFCLK();
  DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_0);

  /*DL_GPIO_initPeripheralOutputFunctionFeatures(CLKOUT_IOMUX,
      CLKOUT_IOMUX_FUNC, DL_GPIO_INVERSION_DISABLE,
      DL_GPIO_RESISTOR_NONE, DL_GPIO_DRIVE_STRENGTH_HIGH,
      DL_GPIO_HIZ_DISABLE);
  DL_GPIO_enableOutput(CLKOUT_PORT, CLKOUT_PIN);
  DL_SYSCTL_enableExternalClock(DL_SYSCTL_CLK_OUT_SOURCE_ULPCLK,
              DL_SYSCTL_CLK_OUT_DIVIDE_DISABLE);*/
}

void pwm_on(void)
{
  DL_TimerA_enablePower(PWM_INSTANCE);
  delay_cycles(POWER_STARTUP_DELAY);
  DL_GPIO_initPeripheralOutputFunction(PWM_IOMUX,PWM_IOMUX_FUNC);
  DL_GPIO_enableOutput(PWM_PORT, PWM_PIN);

  DL_TimerA_setClockConfig(PWM_INSTANCE, (DL_TimerA_ClockConfig *) &gPWM_ClockConfig);

  DL_TimerA_initPWMMode(PWM_INSTANCE, (DL_TimerA_PWMConfig *) &gPWM_Config);

  // Set Counter control to the smallest CC index being used
  DL_TimerA_setCounterControl(PWM_INSTANCE,PWM_ZCOND,PWM_ACOND,PWM_LCOND);

  DL_TimerA_setCaptureCompareOutCtl(PWM_INSTANCE, DL_TIMER_CC_OCTL_INIT_VAL_LOW,
              DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
              PWM_COMPARE_INDEX);

  DL_TimerA_setCaptCompUpdateMethod(PWM_INSTANCE, DL_TIMER_CC_UPDATE_METHOD_IMMEDIATE, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
  DL_TimerA_setCaptureCompareValue(PWM_INSTANCE, 0, PWM_CC_INDEX);

  DL_TimerA_enableClock(PWM_INSTANCE);

  DL_TimerA_setCCPDirection(PWM_INSTANCE, PWM_CC_OUTPUT );
}

void pwm_off(void)
{
  DL_TimerA_setCaptureCompareValue(PWM_INSTANCE, 0, PWM_CC_INDEX);
  DL_GPIO_disableOutput(PWM_PORT, PWM_PIN);
  DL_TimerA_disablePower(PWM_INSTANCE);
}

static void PERIODIC_TIMER_Init()
{
  DL_TimerG_setClockConfig(PERIODIC_TIMER_INSTANCE, (DL_TimerG_ClockConfig *) &gPERIODIC_TIMER_ClockConfig);

  DL_TimerG_initTimerMode(PERIODIC_TIMER_INSTANCE, (DL_TimerG_TimerConfig *) &gPERIODIC_TIMER_Config);

  DL_TimerG_enableInterrupt(PERIODIC_TIMER_INSTANCE, DL_TIMERG_INTERRUPT_ZERO_EVENT);
  DL_TimerG_enableClock(PERIODIC_TIMER_INSTANCE);

  NVIC_SetPriority(PERIODIC_TIMER_IRQn, PERIODIC_TIMER_INTERRUPT_PRIORITY);
  NVIC_ClearPendingIRQ(PERIODIC_TIMER_IRQn);
  NVIC_EnableIRQ(PERIODIC_TIMER_IRQn);
}

static void VREFInit(void)
{
  DL_VREF_setClockConfig(VREF, (DL_VREF_ClockConfig *) &gVREFClockConfig);
  DL_VREF_configReference(VREF, (DL_VREF_Config *) &gVREFConfig);
  delay_cycles(VREF_READY_DELAY);
}

static void ADCInit(void)
{
  DL_ADC12_setClockConfig(ADC_INST, (DL_ADC12_ClockConfig *) &gADC_ClockConfig);
  DL_ADC12_initSingleSample(ADC_INST,
      DL_ADC12_REPEAT_MODE_ENABLED, DL_ADC12_SAMPLING_SOURCE_AUTO, DL_ADC12_TRIG_SRC_SOFTWARE,
      DL_ADC12_SAMP_CONV_RES_12_BIT, DL_ADC12_SAMP_CONV_DATA_FORMAT_UNSIGNED);
  DL_ADC12_configConversionMem(ADC_INST, ADC_ADCMEM_0,
      DL_ADC12_INPUT_CHAN_13, DL_ADC12_REFERENCE_VOLTAGE_INTREF, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
      DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
  //DL_ADC12_enableFIFO(ADC_INST);
  DL_ADC12_setPowerDownMode(ADC_INST,DL_ADC12_POWER_DOWN_MODE_MANUAL);
  DL_ADC12_setSampleTime0(ADC_INST,1023); // maximum
  /* Enable ADC12 interrupt */
  DL_ADC12_clearInterruptStatus(ADC_INST,(DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED));
  DL_ADC12_enableInterrupt(ADC_INST,(DL_ADC12_INTERRUPT_MEM0_RESULT_LOADED));
  DL_ADC12_enableConversions(ADC_INST);
  /* Setup interrupts on device */
  NVIC_SetPriority(ADC_INST_INT_IRQN, ADC_INTERRUPT_PRIORITY);
  NVIC_ClearPendingIRQ(ADC_INST_INT_IRQN);
  NVIC_EnableIRQ(ADC_INST_INT_IRQN);
}

/*
 *OPA1_IN1-=PA17
 */
static void OPA1Init(void)
{
  DL_OPA_init(OPA_INST, &gOPA1Config);
  DL_OPA_setGainBandwidth(OPA_INST, DL_OPA_GBW_LOW);
  DL_OPA_enable(OPA_INST);
}

static void DACInit(void)
{
  DL_DAC12_init(DAC0, &gDAC12Config);
  DL_DAC12_enable(DAC0);
  DL_DAC12_performSelfCalibrationBlocking(DAC0);
}

void SystemInit(void)
{
  SYSCTLInit();
  InitPower();
  GPIOInit();
  UARTInit();
  VREFInit();
  ADCInit();
  DACInit();
  OPA1Init();
  PERIODIC_TIMER_Init();
}

unsigned int dac_get(void)
{
  return DAC0->DATA0;
}

void dac_set(unsigned int value)
{
  DL_DAC12_output12(DAC0, value);
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
  DL_TimerA_setLoadValue(PWM_INSTANCE, arr - 1);
  DL_TimerA_setCaptureCompareValue(PWM_INSTANCE, arr * duty / 100, PWM_CC_INDEX);
}

void puts_(const char *s)
{
  for (;;)
  {
    char c = *s++;
    if (!c)
      return;
    DL_UART_Main_transmitDataBlocking(UART_INSTANCE, c);
  }
}
