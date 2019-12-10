#include <board.h>
#include <vl6180.h>

unsigned int vl6180_check(void)
{
  unsigned char modelId;

  if (!vl6180ReadRegister(VL6180_REG_IDENTIFICATION_MODEL_ID, &modelId, 1))
    return 0;

  return modelId == 0xB4;
}

unsigned int vl6180_reset(void)
{
  unsigned char v;

  VL6180_IO0_LOW;
  delay(1);
  VL6180_IO0_HIGH;
  delayms(2);
  
  while (1)
  {
    if (!vl6180ReadRegister(VL6180_REG_SYSTEM_FRESH_OUT_OF_RESET, &v, 1))
      return 0;
    if (v)
      break;
    else
      delayms(1);
  }

  return 1;
}

unsigned int vl6180_init(vl6180_cfg *cfg)
{
  unsigned char v;

  v = 0x10; //GPIO Interrupt output
  if (!vl6180WriteRegister(VL6180_REG_SYSTEM_MODE_GPIO1, &v, 1))
    return 0;
  if (!vl6180WriteRegister(VL6180_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, &cfg->interruptConfig, 1))
    return 0;
  if (!vl6180WriteRegister(VL6180_REG_SYSRANGE_THRESH_HIGH, &cfg->sysrangeThresholdHigh, 1))
    return 0;
  if (!vl6180WriteRegister(VL6180_REG_SYSRANGE_THRESH_LOW, &cfg->sysrangeThresholdLow, 1))
    return 0;
  if (!vl6180WriteRegister(VL6180_REG_SYSRANGE_INTERMEASUREMENT_PERIOD, &cfg->sysrangeIntermeasurementPeriod, 1))
    return 0;
  return 1;
}

unsigned int vl6180_interruptClear(unsigned char v)
{
  return vl6180WriteRegister(VL6180_REG_SYSTEM_INTERRUPT_CLEAR, &v, 1);
}

unsigned int vl6180_sysRangeStart(unsigned char v)
{
  return vl6180WriteRegister(VL6180_REG_SYSRANGE_START, &v, 1);
}
