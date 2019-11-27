#include <board.h>
#include <uart.h>

#define UART_DIV_SAMPLING16(_PCLK_, _BAUD_)            (((_PCLK_)*25U)/(4U*(_BAUD_)))
#define UART_DIVMANT_SAMPLING16(_PCLK_, _BAUD_)        (UART_DIV_SAMPLING16((_PCLK_), (_BAUD_))/100U)
#define UART_DIVFRAQ_SAMPLING16(_PCLK_, _BAUD_)        (((UART_DIV_SAMPLING16((_PCLK_), (_BAUD_)) - (UART_DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) * 100U)) * 16U + 50U) / 100U)
/* UART BRR = mantissa + overflow + fraction
            = (UART DIVMANT << 4) + (UART DIVFRAQ & 0xF0) + (UART DIVFRAQ & 0x0FU) */
#define UART_BRR_SAMPLING16(_PCLK_, _BAUD_)            (((UART_DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) << 4U) + \
                                                        (UART_DIVFRAQ_SAMPLING16((_PCLK_), (_BAUD_)) & 0xF0U)) + \
                                                        (UART_DIVFRAQ_SAMPLING16((_PCLK_), (_BAUD_)) & 0x0FU))

#define UART_DIV_SAMPLING8(_PCLK_, _BAUD_)             (((_PCLK_)*25U)/(2U*(_BAUD_)))
#define UART_DIVMANT_SAMPLING8(_PCLK_, _BAUD_)         (UART_DIV_SAMPLING8((_PCLK_), (_BAUD_))/100U)
#define UART_DIVFRAQ_SAMPLING8(_PCLK_, _BAUD_)         (((UART_DIV_SAMPLING8((_PCLK_), (_BAUD_)) - (UART_DIVMANT_SAMPLING8((_PCLK_), (_BAUD_)) * 100U)) * 8U + 50U) / 100U)
/* UART BRR = mantissa + overflow + fraction
            = (UART DIVMANT << 4) + ((UART DIVFRAQ & 0xF8) << 1) + (UART DIVFRAQ & 0x07U) */
#define UART_BRR_SAMPLING8(_PCLK_, _BAUD_)             (((UART_DIVMANT_SAMPLING8((_PCLK_), (_BAUD_)) << 4U) + \
                                                        ((UART_DIVFRAQ_SAMPLING8((_PCLK_), (_BAUD_)) & 0xF8U) << 1U)) + \
                                                        (UART_DIVFRAQ_SAMPLING8((_PCLK_), (_BAUD_)) & 0x07U))

#define UART_FLAG_CTS                       USART_SR_CTS
#define UART_FLAG_LBD                       USART_SR_LBD
#define UART_FLAG_TXE                       USART_SR_TXE
#define UART_FLAG_TC                        USART_SR_TC
#define UART_FLAG_RXNE                      USART_SR_RXNE
#define UART_FLAG_IDLE                      USART_SR_IDLE
#define UART_FLAG_ORE                       USART_SR_ORE
#define UART_FLAG_NE                        USART_SR_NE
#define UART_FLAG_FE                        USART_SR_FE
#define UART_FLAG_PE                        USART_SR_PE

/**
  * @brief  Configures the USART peripheral.
  * @param  husart Pointer to a USART_HandleTypeDef structure that contains
  *                the configuration information for the specified USART module.
  * @retval None
  */
void UART_Init(USART_TypeDef *instance, UART_InitTypeDef *init)
{
  uint32_t tmpreg;

  /*-------------------------- USART CR2 Configuration -----------------------*/
  /* Configure the UART Stop Bits: Set STOP[13:12] bits
     according to init->StopBits value */
  MODIFY_REG(instance->CR2, USART_CR2_STOP, init->StopBits);

  /*-------------------------- USART CR1 Configuration -----------------------*/
  /* Configure the UART Word Length, Parity and mode:
     Set the M bits according to init->WordLength value
     Set PCE and PS bits according to init->Parity value
     Set TE and RE bits according to init->Mode value
     Set OVER8 bit according to init->OverSampling value */

  tmpreg = (uint32_t)init->WordLength | init->Parity | init->Mode | init->OverSampling;
  MODIFY_REG(instance->CR1,
             (uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
             tmpreg);

  /*-------------------------- USART CR3 Configuration -----------------------*/
  /* Configure the UART HFC: Set CTSE and RTSE bits according to init->HwFlowCtl value */
  MODIFY_REG(instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE), init->HwFlowCtl);

  /* Check the Over Sampling */
  if(init->OverSampling == UART_OVERSAMPLING_8)
  {
    /*------- UART-associated USART registers setting : BRR Configuration ------*/
    if((instance == USART1))
    {
      instance->BRR = UART_BRR_SAMPLING8(PCLK2_FREQ, init->BaudRate);
    }
    else
    {
      instance->BRR = UART_BRR_SAMPLING8(PCLK1_FREQ, init->BaudRate);
    }
  }
  else
  {
    /*------- UART-associated USART registers setting : BRR Configuration ------*/
    if((instance == USART1))
    {
      instance->BRR = UART_BRR_SAMPLING16(PCLK2_FREQ, init->BaudRate);
    }
    else
    {
      instance->BRR = UART_BRR_SAMPLING16(PCLK1_FREQ, init->BaudRate);
    }
  }

  CLEAR_BIT(instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* Enable the peripheral */
  instance->CR1 |=  USART_CR1_UE;
}

static unsigned int UART_WaitOnFlagUntilTimeout(USART_TypeDef *instance, unsigned int flag, unsigned int status, unsigned int timeout)
{
  while (--timeout && (instance->SR & flag) != status)
    ;
  return timeout;
}

unsigned int UART_Send(USART_TypeDef *instance, unsigned char c)
{
  unsigned int rc = UART_WaitOnFlagUntilTimeout(instance, UART_FLAG_TXE, UART_FLAG_TXE, 0xFFFFFF);
  if (!rc)
  {
    return 0;
  }
  instance->DR = c;
  return 1;
}

unsigned int UART_Receive(USART_TypeDef *instance, unsigned char *c)
{
  if (!(instance->SR & UART_FLAG_RXNE))
    return 0;
  *c = instance->DR & 0xFFU;
  return 1;
}
