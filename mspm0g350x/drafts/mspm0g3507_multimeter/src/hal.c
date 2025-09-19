#include "board.h"
#include <dl_gpio.h>
#include <m0p/dl_sysctl.h>
#include <dl_uart_main.h>
#include <dl_vref.h>
#include <dl_dac12.h>
#include <dl_opa.h>
#include <dl_i2c.h>
#include <limits.h>
#include <mcp3421.h>
#include <systick.h>
#include <dl_flashctl.h>

static const DL_UART_Main_ClockConfig gUART_0ClockConfig = {
  .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
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

static const DL_VREF_ClockConfig gVREFClockConfig = {
  .clockSel = DL_VREF_CLOCK_BUSCLK,
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
  .sampleRate                = DL_DAC12_SAMPLES_PER_SECOND_500,
};

static const DL_OPA_Config gOPA_0Config0 = {
  .pselChannel    = DL_OPA_PSEL_DAC_OUT,
  .nselChannel    = DL_OPA_NSEL_IN1_NEG,
  .mselChannel    = DL_OPA_MSEL_OPEN,
  .gain           = DL_OPA_GAIN_N0_P1,
  .outputPinState = DL_OPA_OUTPUT_PIN_ENABLED,
  .choppingMode   = DL_OPA_CHOPPING_MODE_DISABLE
};

static const DL_I2C_ClockConfig gI2CClockConfig = {
  .clockSel = DL_I2C_CLOCK_BUSCLK,
  .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
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
  DL_UART_Main_setBaudRateDivisor(UART_INSTANCE, UART_IBRD_32_MHZ_115200_BAUD, UART_FBRD_32_MHZ_115200_BAUD);

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
  DL_VREF_reset(VREF);
  DL_DAC12_reset(DAC0);
  DL_I2C_reset(I2C1);
  DL_OPA_reset(OPA0);

  DL_GPIO_enablePower(GPIOA);
  DL_UART_Main_enablePower(UART_INSTANCE);
  DL_VREF_enablePower(VREF);
  DL_DAC12_enablePower(DAC0);
  DL_I2C_enablePower(I2C1);
  DL_OPA_enablePower(OPA0);
  delay_cycles(POWER_STARTUP_DELAY);
}

static void SYSCTLInit(void)
{
  //Low Power Mode is configured to be STANDBY0
  DL_SYSCTL_setPowerPolicyRUN0SLEEP0();
  DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_3);

  DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
  /* Set default configuration */
  DL_SYSCTL_disableHFXT();
  DL_SYSCTL_disableSYSPLL();
  // SYSOSC = 32 MHz = 1 wait state
  DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_1);
}

static void VREFInit(void)
{
  DL_VREF_setClockConfig(VREF, (DL_VREF_ClockConfig *) &gVREFClockConfig);
  DL_VREF_configReference(VREF, (DL_VREF_Config *) &gVREFConfig);
}

static void DACInit(void)
{
  DL_DAC12_init(DAC0, &gDAC12Config);
  DL_DAC12_enable(DAC0);
  DL_DAC12_performSelfCalibrationBlocking(DAC0);
}

/*
 *OPA0_OUT=PA22
 *OPA0_IN1-=PA24
 */
static void OPA0Init(void)
{
  DL_OPA_init(OPA0, &gOPA_0Config0);
  DL_OPA_setGainBandwidth(OPA0, DL_OPA_GBW_LOW);
  DL_OPA_enable(OPA0);
}

/*
 *SCL=PA17
 *SDA=PA18
 */
static void I2C1Init(void)
{
  DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SDA,
      GPIO_I2C_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
      DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
      DL_GPIO_WAKEUP_DISABLE);
  DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IOMUX_SCL,
      GPIO_I2C_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
      DL_GPIO_RESISTOR_PULL_UP, DL_GPIO_HYSTERESIS_DISABLE,
      DL_GPIO_WAKEUP_DISABLE);
  DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SDA);
  DL_GPIO_enableHiZ(GPIO_I2C_IOMUX_SCL);

  DL_I2C_setClockConfig(I2C1, (DL_I2C_ClockConfig *) &gI2CClockConfig);
  DL_I2C_disableAnalogGlitchFilter(I2C1);

  /* Configure Controller Mode */
  DL_I2C_resetControllerTransfer(I2C1);
  /* Set frequency to 400000 Hz*/
  DL_I2C_setTimerPeriod(I2C1, 7);
  DL_I2C_setControllerTXFIFOThreshold(I2C1, DL_I2C_TX_FIFO_LEVEL_EMPTY);
  DL_I2C_setControllerRXFIFOThreshold(I2C1, DL_I2C_RX_FIFO_LEVEL_BYTES_1);
  DL_I2C_enableControllerClockStretching(I2C1);

  /* Enable module */
  DL_I2C_enableController(I2C1);
}

void SystemInit(void)
{
  SYSCTLInit();
  InitPower();
  GPIOInit();
  UARTInit();
  VREFInit();
  DACInit();
  OPA0Init();
  I2C1Init();
}

unsigned int mv_to_12(unsigned int mv)
{
  if (mv >= DAC_REFERENCE_VOLTAGE)
    return 4095;
  return (mv * 4095) / DAC_REFERENCE_VOLTAGE;
}

int mcp3421Read(int channel, unsigned char address, unsigned char *data, unsigned int l)
{
  unsigned int timeout = I2C_TIMEOUT;
  /* Wait for I2C to be Idle */
  while (timeout)
  {
    if (DL_I2C_getControllerStatus(I2C1) & DL_I2C_CONTROLLER_STATUS_IDLE)
      break;
    timeout--;
  }
  if (!timeout)
    return 5;

  /* Send a read request to Target */
  DL_I2C_startControllerTransfer(I2C1, address, DL_I2C_CONTROLLER_DIRECTION_RX, l);

  /*
   * Receive all bytes from target. LED will remain high if not all bytes
   * are received
   */
  for (unsigned int i = 0; i < l; i++)
  {
    timeout = I2C_TIMEOUT;
    while (timeout)
    {
      if (!DL_I2C_isControllerRXFIFOEmpty(I2C1))
        break;
      timeout--;
    }
    if (!timeout)
    {
      DL_I2C_enableStopCondition(I2C1);
      return 6;
    }
    *data++ = DL_I2C_receiveControllerData(I2C1);
  }

  return 0;
}

int mcp3421Write(int channel, unsigned char address, unsigned char data)
{
  DL_I2C_fillControllerTXFIFO(I2C1, &data, 1);

  unsigned int timeout = I2C_TIMEOUT;
  /* Wait for I2C to be Idle */
  while (timeout)
  {
    if (DL_I2C_getControllerStatus(I2C1) & DL_I2C_CONTROLLER_STATUS_IDLE)
      break;
    timeout--;
  }
  if (!timeout)
    return 1;

  /* Send the packet to the controller.
   * This function will send Start + Stop automatically.
   */
  DL_I2C_startControllerTransfer(I2C1, address, DL_I2C_CONTROLLER_DIRECTION_TX, 1);

  timeout = I2C_TIMEOUT;
  /* Poll until the Controller writes all bytes */
  while (timeout)
  {
    if (!(DL_I2C_getControllerStatus(I2C1) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS))
      break;
    timeout--;
  }
  if (!timeout)
  {
    DL_I2C_enableStopCondition(I2C1);
    return 2;
  }

  /* Trap if there was an error */
  if (DL_I2C_getControllerStatus(I2C1) & DL_I2C_CONTROLLER_STATUS_ERROR)
  {
    DL_I2C_enableStopCondition(I2C1);
    return 3;
  }

  timeout = I2C_TIMEOUT;
  /* Wait for I2C to be Idle */
  while (timeout)
  {
    if (DL_I2C_getControllerStatus(I2C1) & DL_I2C_CONTROLLER_STATUS_IDLE)
      break;
    timeout--;
  }
  if (!timeout)
  {
    DL_I2C_enableStopCondition(I2C1);
    return 4;
  }

  return 0;
}

int adc_read(int gain, int *value_uv)
{
  if (gain < 0 || gain > ADC_MAX_GAIN)
    return 1;

  MCP3421Config dcfg = {
    .start_conversion = 1,
    .continuous_conversion = 0,
    .sample_rate = MCP3421_RATE_4,
    .gain = gain
  };
  int rc = mcp3421SetConfig(0, MCP3421_DEVICE_ID>>1, &dcfg);
  if (rc)
    return rc;

  delayms(300);

  int value;
  rc = mcp3421Get18BitVoltage(0, MCP3421_DEVICE_ID>>1, &value);
  if (rc)
    return rc;

  if (value == ADC_MAX_VALUE)
    *value_uv = INT_MAX;
  else if (value == ADC_MIN_VALUE)
    *value_uv = INT_MIN;
  else
    *value_uv = (int)((long long int)value * ADC_REFERENCE_VOLTAGE / ADC_MAX_VALUE);

  return 0;
}

static unsigned int mv_from_12(unsigned int code)
{
  if (code >= 255)
    return DAC_REFERENCE_VOLTAGE;
  return code * DAC_REFERENCE_VOLTAGE / 4096;
}


unsigned int dac_get(void)
{
  mv_from_12(DAC0->DATA0);
}

void dac_set(unsigned int value)
{
  DL_DAC12_output12(DAC0, mv_to_12(value));
}

int eeprom_write(void *data, unsigned int size)
{
  DL_FLASHCTL_COMMAND_STATUS FlashAPIState;

  /* Set the new record's header to Recording */
  DL_FlashCTL_executeClearStatus(FLASHCTL);

  unsigned int sz = size;
  unsigned int address = EEPROM_START;
  while (sz)
  {
    DL_FlashCTL_unprotectSector(
        FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
    FlashAPIState = DL_FlashCTL_eraseMemoryFromRAM(
          FLASHCTL, address, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
    if (FlashAPIState == DL_FLASHCTL_COMMAND_STATUS_FAILED)
      return 1;
    if (sz >= DL_FLASHCTL_COMMAND_SIZE_SECTOR)
      sz -= DL_FLASHCTL_COMMAND_SIZE_SECTOR;
    else
      sz = 0;
    address += DL_FLASHCTL_COMMAND_SIZE_SECTOR;
  }

#ifdef __MSPM0_HAS_ECC__
  FlashAPIState = DL_FlashCTL_programMemoryBlockingFromRAM64WithECCGenerated(
      FLASHCTL, EEPROM_START, data, size / sizeof(uint32_t) + 2, DL_FLASHCTL_REGION_SELECT_MAIN);
#else
  FlashAPIState = DL_FlashCTL_programMemoryFromRAM(
      FLASHCTL, EEPROM_START, data, size / sizeof(uint32_t) + 2, DL_FLASHCTL_REGION_SELECT_MAIN);
#endif
  if (FlashAPIState == DL_FLASHCTL_COMMAND_STATUS_FAILED)
    return 2;

  return 0;
}