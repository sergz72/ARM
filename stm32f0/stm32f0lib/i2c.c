#include <board.h>
#include <i2c.h>
#include <nvic.h>

static I2C_InitTypeDef *config[2];

void I2C_Init(I2C_TypeDef* i2c, const I2C_InitTypeDef* init)
{
  unsigned int cr = 0;

  if (i2c == I2C1)
    config[0] = (I2C_InitTypeDef*)init;
  else
    config[1] = (I2C_InitTypeDef*)init;

  i2c->TIMINGR = init->Timing;

  if (init->OwnAddress1 != 0)
  {
    if (init->OwnAddress1 > 0x7F)
      cr = init->OwnAddress1 | I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE;
    else
      cr = (init->OwnAddress1 << 1) | I2C_OAR1_OA1EN;
  }
  i2c->OAR1 = cr;
  cr = 0;

  if (init->OwnAddress2 != 0)
    cr = init->OwnAddress2 | (init->OwnAddress2Mask << 8) | I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE;
  i2c->OAR2 = cr;

  i2c->CR2 = 0;

  cr = init->DigitalFilter << 8;

  if (init->TXISHandler)
    cr |= I2C_CR1_TXIE;
  if (init->RXNEHandler)
    cr |= I2C_CR1_RXIE;
  if (init->AddressInterruptEnable)
    cr |= I2C_CR1_ADDRIE;
  if (init->NAckInterruptEnable)
    cr |= I2C_CR1_NACKIE;
  if (init->StopInterruptEnable)
    cr |= I2C_CR1_STOPIE;
  if (init->TCHandler)
    cr |= I2C_CR1_TCIE;
  if (init->ErrorInterruptsEnable)
    cr |= I2C_CR1_ERRIE;
  if (!init->AnalogFilterEnable)
    cr |= I2C_CR1_ANFOFF;
  if (init->TXDmaEnable)
    cr |= I2C_CR1_TXDMAEN;
  if (init->RXDmaEnable)
    cr |= I2C_CR1_RXDMAEN;
  if (init->SlaveByteControlEnable)
    cr |= I2C_CR1_SBC;
  if (!init->ClockStretchingEnable)
    cr |= I2C_CR1_NOSTRETCH;
  if (init->GeneralCallEnable)
    cr |= I2C_CR1_GCEN;

  i2c->CR1 = cr;

#ifdef I2C1_IRQ_ENABLE
  NVIC_Init(I2C1_IRQn, 3, 1, ENABLE);
#endif
#ifdef I2C2_IRQ_ENABLE
  NVIC_Init(I2C2_IRQn, 3, 1, ENABLE);
#endif

  i2c->CR1 |= I2C_CR1_PE;
}

static void I2C_IRQHandler(I2C_TypeDef* i2c, int configId)
{
  unsigned int flags = i2c->ISR;
  I2C_InitTypeDef *c = config[configId];

  if (flags & I2C_ISR_ADDR)
  {
    if (c->ADDRHandler)
      c->ADDRHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_ADDRCF;
  }
  if (flags & I2C_ISR_STOPF)
  {
    if (c->STOPHandler)
      c->STOPHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_STOPCF;
  }
  if (flags & I2C_ISR_NACKF)
  {
    if (c->NACKHandler)
      c->NACKHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_NACKCF;
  }
  if (flags & I2C_ISR_TC)
    c->TCHandler(i2c);
  if (flags & I2C_ISR_TCR)
  {
    if (c->TCRHandler)
      c->TCRHandler(i2c);
  }
  if (flags & I2C_ISR_BERR)
  {
    if (c->BERRHandler)
      c->BERRHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_BERRCF;
  }
  if (flags & I2C_ISR_ARLO)
  {
    if (c->ARLOHandler)
      c->ARLOHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_ARLOCF;
  }
  if (flags & I2C_ISR_OVR)
  {
    if (c->OVRHandler)
      c->OVRHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_OVRCF;
  }
  if (flags & I2C_ISR_PECERR)
  {
    if (c->PECERRHandler)
      c->PECERRHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_PECCF;
  }
  if (flags & I2C_ISR_TIMEOUT)
  {
    if (c->TimeoutHandler)
      c->TimeoutHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_TIMOUTCF;
  }
  if (flags & I2C_ISR_ALERT)
  {
    if (c->AlertHandler)
      c->AlertHandler(i2c);
    else
      I2C1->ICR = I2C_ICR_ALERTCF;
  }
  if (flags & I2C_ISR_TXIS)
    c->TXISHandler(i2c);
  if (flags & I2C_ISR_RXNE)
    c->RXNEHandler(i2c);
}

#ifdef I2C1_IRQ_ENABLE
void I2C1_IRQHandler(void)
{
  I2C_IRQHandler(I2C1, 0);
}
#endif

#ifdef I2C2_IRQ_ENABLE
void I2C2_IRQHandler(void)
{
  I2C_IRQHandler(I2C2, 1);
}
#endif