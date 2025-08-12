#include <board.h>
#include <vl6180.h>

static int vl6180WriteByte(unsigned short reg, unsigned char value)
{
  return vl6180WriteRegister(reg, &value, 1);
}

static int vl6180WriteShort(unsigned short reg, unsigned short value)
{
  unsigned char data[2] = { (unsigned char)(value >> 8), (unsigned char)value };
  return vl6180WriteRegister(reg, data, 2);
}

int vl6180_check(void)
{
  unsigned char modelId;

  if (!vl6180ReadRegister(VL6180_REG_IDENTIFICATION_MODEL_ID, &modelId, 1))
    return 0;

  return modelId != 0xB4;
}

int vl6180_reset(void)
{
  unsigned char v;

  VL6180_IO0_LOW;
  delay(1);
  VL6180_IO0_HIGH;
  delayms(2);

  while (1)
  {
    int rc = vl6180ReadRegister(VL6180_REG_SYSTEM_FRESH_OUT_OF_RESET, &v, 1);
    if (rc)
      return rc;
    if (v)
      break;
    delayms(1);
  }

  return 0;
}

int vl6180_init(vl6180_cfg *cfg)
{
  int rc = vl6180_reset();
  if (rc)
    return rc;
  rc = vl6180_check();
  if (rc)
    return rc;

  // Mandatory : private registers
  rc = vl6180WriteByte(0x0702, 0x01);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x0802, 0x01);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x9600, 0x00);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x9700, 0xfd);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xe300, 0x01);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xe400, 0x03);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xe500, 0x02);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xe600, 0x01);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xe700, 0x03);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xf500, 0x02);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xd900, 0x05);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xdb00, 0xce);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xdc00, 0x03);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xdd00, 0xf8);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x9f00, 0x00);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xa300, 0x3c);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xb700, 0x00);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xbb00, 0x3c);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xb200, 0x09);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xca00, 0x09);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x9801, 0x01);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xb001, 0x17);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xad01, 0x00);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xff00, 0x05);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x0001, 0x05);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x9901, 0x05);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xa601, 0x1b);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xac01, 0x3e);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0xa701, 0x1f);
  if (rc)
    return rc;
  rc = vl6180WriteByte(0x3000, 0x00);
  if (rc)
    return rc;

  rc = vl6180WriteByte(VL6180_REG_SYSTEM_MODE_GPIO1, 0x10);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSTEM_INTERRUPT_CONFIG_GPIO, cfg->interruptConfig);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSRANGE_THRESH_HIGH, cfg->sysrangeThresholdHigh);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSRANGE_THRESH_LOW, cfg->sysrangeThresholdLow);
  if (rc)
    return rc;
  rc = vl6180WriteShort(VL6180_REG_SYSALS_THRESH_HIGH, cfg->sysalsThresholdHigh);
  if (rc)
    return rc;
  rc = vl6180WriteShort(VL6180_REG_SYSALS_THRESH_LOW, cfg->sysalsThresholdLow);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSRANGE_INTERMEASUREMENT_PERIOD, cfg->sysrangeIntermeasurementPeriod);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSALS_INTERMEASUREMENT_PERIOD, cfg->sysalsIntermeasurementPeriod);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSALS_ANALOGUE_GAIN, 0x40 | cfg->sysalsGain);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSRANGE_VHV_REPEAT_RATE, 0xFF);
  if (rc)
    return rc;
  rc = vl6180WriteByte(VL6180_REG_SYSRANGE_VHV_RECALIBRATE, 1);
  if (rc)
    return rc;
  return vl6180_sysAlsSetIntegrationPeriod(cfg->sysalsIntegrationPeriod);
}

int vl6180_interruptClear(unsigned char v)
{
  return vl6180WriteByte(VL6180_REG_SYSTEM_INTERRUPT_CLEAR, v);
}

int vl6180_sysRangeStart(unsigned char v)
{
  return vl6180WriteByte(VL6180_REG_SYSRANGE_START, v);
}

int vl6180_sysAlsStart(unsigned char v)
{
  return vl6180WriteByte(VL6180_REG_SYSALS_START, v);
}

int vl6180_sysAlsSetAnalogGain(unsigned char gain)
{
  return vl6180WriteByte(VL6180_REG_SYSALS_ANALOGUE_GAIN, (gain & 7) | 0x40);
}

int vl6180_sysAlsSetIntegrationPeriod(unsigned char period)
{
  return vl6180WriteShort(VL6180_REG_SYSALS_INTEGRATION_PERIOD, period);
}

int vl6180_getRangeStatus(unsigned char *status)
{
  return vl6180ReadRegister(VL6180_REG_RESULT_RANGE_STATUS, status, 1);
}

int vl6180_getAlsStatus(unsigned char *status)
{
  return vl6180ReadRegister(VL6180_REG_RESULT_ALS_STATUS, status, 1);
}

int vl6180_getAlsValue(unsigned short *value)
{
  unsigned char data[2];
  int rc = vl6180ReadRegister(VL6180_REG_RESULT_ALS_VAL, data, 2);
  if (rc)
    return rc;
  *value = ((unsigned short)data[0] << 8) | (unsigned short)data[1];
  return 0;
}
