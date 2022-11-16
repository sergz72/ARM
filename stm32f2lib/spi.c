#include <spi.h>

/* SPI SPE mask */
#define CR1_SPE_Set          ((unsigned short)0x0040)
#define CR1_SPE_Reset        ((unsigned short)0xFFBF)

/* I2S I2SE mask */
#define I2SCFGR_I2SE_Set     ((unsigned short)0x0400)
#define I2SCFGR_I2SE_Reset   ((unsigned short)0xFBFF)

/* SPI CRCNext mask */
#define CR1_CRCNext_Set      ((unsigned short)0x1000)

/* SPI CRCEN mask */
#define CR1_CRCEN_Set        ((unsigned short)0x2000)
#define CR1_CRCEN_Reset      ((unsigned short)0xDFFF)

/* SPI SSOE mask */
#define CR2_SSOE_Set         ((unsigned short)0x0004)
#define CR2_SSOE_Reset       ((unsigned short)0xFFFB)

/* SPI registers Masks */
#define CR1_CLEAR_Mask       ((unsigned short)0x3040)
#define I2SCFGR_CLEAR_Mask   ((unsigned short)0xF040)

/* SPI or I2S mode selection masks */
#define SPI_Mode_Select      ((unsigned short)0xF7FF)
#define I2S_Mode_Select      ((unsigned short)0x0800) 

/* I2S clock source selection masks */
#define I2S2_CLOCK_SRC       ((unsigned int)(0x00020000))
#define I2S3_CLOCK_SRC       ((unsigned int)(0x00040000))
#define I2S_MUL_MASK         ((unsigned int)(0x0000F000))
#define I2S_DIV_MASK         ((unsigned int)(0x000000F0))

void SPI_I2S_DeInit(SPI_TypeDef* SPIx)
{
  if (SPIx == SPI1)
  {
    /* Enable SPI1 reset state */
    RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
    /* Release SPI1 from reset state */
    RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
  }
  else if (SPIx == SPI2)
  {
    /* Enable SPI2 reset state */
    RCC->APB1RSTR |= RCC_APB1RSTR_SPI2RST;
    /* Release SPI2 from reset state */
    RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI2RST;
  }
  else if (SPIx == SPI3)
  {
    /* Enable SPI3 reset state */
    RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST;
    /* Release SPI3 from reset state */
    RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST;
  }
}

/**
  * @brief  Initializes the SPIx peripheral according to the specified 
  *   parameters in the SPI_InitStruct.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *   contains the configuration information for the specified SPI peripheral.
  * @retval None
  */
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
  unsigned short tmpreg = 0;

/*---------------------------- SPIx CR1 Configuration ------------------------*/
  /* Get the SPIx CR1 value */
  tmpreg = SPIx->CR1;
  /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits */
  tmpreg &= CR1_CLEAR_Mask;
  /* Configure SPIx: direction, NSS management, first transmitted bit, BaudRate prescaler
     master/salve mode, CPOL and CPHA */
  /* Set BIDImode, BIDIOE and RxONLY bits according to SPI_Direction value */
  /* Set SSM, SSI and MSTR bits according to SPI_Mode and SPI_NSS values */
  /* Set LSBFirst bit according to SPI_FirstBit value */
  /* Set BR bits according to SPI_BaudRatePrescaler value */
  /* Set CPOL bit according to SPI_CPOL value */
  /* Set CPHA bit according to SPI_CPHA value */
  tmpreg |= (unsigned short)((unsigned int)SPI_InitStruct->SPI_Direction | SPI_InitStruct->SPI_Mode |
                  SPI_InitStruct->SPI_DataSize | SPI_InitStruct->SPI_CPOL |  
                  SPI_InitStruct->SPI_CPHA | SPI_InitStruct->SPI_NSS |  
                  SPI_InitStruct->SPI_BaudRatePrescaler | SPI_InitStruct->SPI_FirstBit);
  /* Write to SPIx CR1 */
  SPIx->CR1 = tmpreg;
  
  /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
  SPIx->I2SCFGR &= SPI_Mode_Select;		

/*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Write to SPIx CRCPOLY */
  SPIx->CRCPR = SPI_InitStruct->SPI_CRCPolynomial;
}

/**
  * @brief  Enables or disables the specified SPI peripheral.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  NewState: new state of the SPIx peripheral. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected SPI peripheral */
    SPIx->CR1 |= CR1_SPE_Set;
  }
  else
  {
    /* Disable the selected SPI peripheral */
    SPIx->CR1 &= CR1_SPE_Reset;
  }
}

/**
  * @brief  Enables or disables the specified SPI/I2S interrupts.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  *   - 2 or 3 in I2S mode
  * @param  SPI_I2S_IT: specifies the SPI/I2S interrupt source to be enabled or disabled. 
  *   This parameter can be one of the following values:
  *     @arg SPI_I2S_IT_TXE: Tx buffer empty interrupt mask
  *     @arg SPI_I2S_IT_RXNE: Rx buffer not empty interrupt mask
  *     @arg SPI_I2S_IT_ERR: Error interrupt mask
  * @param  NewState: new state of the specified SPI/I2S interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, unsigned char SPI_I2S_IT, FunctionalState NewState)
{
  unsigned short itpos = 0, itmask = 0 ;

  /* Get the SPI/I2S IT index */
  itpos = SPI_I2S_IT >> 4;

  /* Set the IT mask */
  itmask = (unsigned short)1 << (unsigned short)itpos;

  if (NewState != DISABLE)
  {
    /* Enable the selected SPI/I2S interrupt */
    SPIx->CR2 |= itmask;
  }
  else
  {
    /* Disable the selected SPI/I2S interrupt */
    SPIx->CR2 &= (unsigned short)~itmask;
  }
}

/**
  * @brief  Enables or disables the SPIx/I2Sx DMA interface.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  *   - 2 or 3 in I2S mode
  * @param  SPI_I2S_DMAReq: specifies the SPI/I2S DMA transfer request to be enabled or disabled. 
  *   This parameter can be any combination of the following values:
  *     @arg SPI_I2S_DMAReq_Tx: Tx buffer DMA transfer request
  *     @arg SPI_I2S_DMAReq_Rx: Rx buffer DMA transfer request
  * @param  NewState: new state of the selected SPI/I2S DMA transfer request.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, unsigned short SPI_I2S_DMAReq, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected SPI/I2S DMA requests */
    SPIx->CR2 |= SPI_I2S_DMAReq;
  }
  else
  {
    /* Disable the selected SPI/I2S DMA requests */
    SPIx->CR2 &= (unsigned short)~SPI_I2S_DMAReq;
  }
}

/**
  * @brief  Transmits a Data through the SPIx/I2Sx peripheral.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  *   - 2 or 3 in I2S mode
  * @param  Data : Data to be transmitted.
  * @retval None
  */
void SPI_I2S_SendData(SPI_TypeDef* SPIx, unsigned short Data)
{
  /* Write in the DR register the data to be sent */
  SPIx->DR = Data;
}

/**
  * @brief  Returns the most recent received data by the SPIx/I2Sx peripheral. 
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  *   - 2 or 3 in I2S mode
  * @retval The value of the received data.
  */
unsigned short SPI_I2S_ReceiveData(SPI_TypeDef* SPIx)
{
  /* Return the data in the DR register */
  return SPIx->DR;
}

/**
  * @brief  Configures internally by software the NSS pin for the selected SPI.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_NSSInternalSoft: specifies the SPI NSS internal state.
  *   This parameter can be one of the following values:
  *     @arg SPI_NSSInternalSoft_Set: Set NSS pin internally
  *     @arg SPI_NSSInternalSoft_Reset: Reset NSS pin internally
  * @retval None
  */
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, unsigned short SPI_NSSInternalSoft)
{
  if (SPI_NSSInternalSoft != SPI_NSSInternalSoft_Reset)
  {
    /* Set NSS pin internally by software */
    SPIx->CR1 |= SPI_NSSInternalSoft_Set;
  }
  else
  {
    /* Reset NSS pin internally by software */
    SPIx->CR1 &= SPI_NSSInternalSoft_Reset;
  }
}

/**
  * @brief  Enables or disables the SS output for the selected SPI.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  NewState: new state of the SPIx SS output. 
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected SPI SS output */
    SPIx->CR2 |= CR2_SSOE_Set;
  }
  else
  {
    /* Disable the selected SPI SS output */
    SPIx->CR2 &= CR2_SSOE_Reset;
  }
}

/**
  * @brief  Configures the data size for the selected SPI.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_DataSize: specifies the SPI data size.
  *   This parameter can be one of the following values:
  *     @arg SPI_DataSize_16b: Set data frame format to 16bit
  *     @arg SPI_DataSize_8b: Set data frame format to 8bit
  * @retval None
  */
void SPI_DataSizeConfig(SPI_TypeDef* SPIx, unsigned short SPI_DataSize)
{
  /* Clear DFF bit */
  SPIx->CR1 &= (unsigned short)~SPI_DataSize_16b;
  /* Set new DFF bit value */
  SPIx->CR1 |= SPI_DataSize;
}

/**
  * @brief  Transmit the SPIx CRC value.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @retval None
  */
void SPI_TransmitCRC(SPI_TypeDef* SPIx)
{
  /* Enable the selected SPI CRC transmission */
  SPIx->CR1 |= CR1_CRCNext_Set;
}

/**
  * @brief  Enables or disables the CRC value calculation of the transfered bytes.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  NewState: new state of the SPIx CRC value calculation.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_CalculateCRC(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected SPI CRC calculation */
    SPIx->CR1 |= CR1_CRCEN_Set;
  }
  else
  {
    /* Disable the selected SPI CRC calculation */
    SPIx->CR1 &= CR1_CRCEN_Reset;
  }
}

/**
  * @brief  Returns the transmit or the receive CRC register value for the specified SPI.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_CRC: specifies the CRC register to be read.
  *   This parameter can be one of the following values:
  *     @arg SPI_CRC_Tx: Selects Tx CRC register
  *     @arg SPI_CRC_Rx: Selects Rx CRC register
  * @retval The selected CRC register value..
  */
unsigned short SPI_GetCRC(SPI_TypeDef* SPIx, unsigned char SPI_CRC)
{
  unsigned short crcreg = 0;

  if (SPI_CRC != SPI_CRC_Rx)
  {
    /* Get the Tx CRC register */
    crcreg = SPIx->TXCRCR;
  }
  else
  {
    /* Get the Rx CRC register */
    crcreg = SPIx->RXCRCR;
  }
  /* Return the selected CRC register */
  return crcreg;
}

/**
  * @brief  Returns the CRC Polynomial register value for the specified SPI.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @retval The CRC Polynomial register value.
  */
unsigned short SPI_GetCRCPolynomial(SPI_TypeDef* SPIx)
{
  /* Return the CRC polynomial register */
  return SPIx->CRCPR;
}

/**
  * @brief  Selects the data transfer direction in bi-directional mode for the specified SPI.
  * @param  SPIx: where x can be 1, 2 or 3 to select the SPI peripheral.
  * @param  SPI_Direction: specifies the data transfer direction in bi-directional mode. 
  *   This parameter can be one of the following values:
  *     @arg SPI_Direction_Tx: Selects Tx transmission direction
  *     @arg SPI_Direction_Rx: Selects Rx receive direction
  * @retval None
  */
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, unsigned short SPI_Direction)
{
  if (SPI_Direction == SPI_Direction_Tx)
  {
    /* Set the Tx only mode */
    SPIx->CR1 |= SPI_Direction_Tx;
  }
  else
  {
    /* Set the Rx only mode */
    SPIx->CR1 &= SPI_Direction_Rx;
  }
}

/**
  * @brief  Checks whether the specified SPI/I2S flag is set or not.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  *   - 2 or 3 in I2S mode
  * @param  SPI_I2S_FLAG: specifies the SPI/I2S flag to check. 
  *   This parameter can be one of the following values:
  *     @arg SPI_I2S_FLAG_TXE: Transmit buffer empty flag.
  *     @arg SPI_I2S_FLAG_RXNE: Receive buffer not empty flag.
  *     @arg SPI_I2S_FLAG_BSY: Busy flag.
  *     @arg SPI_I2S_FLAG_OVR: Overrun flag.
  *     @arg SPI_FLAG_MODF: Mode Fault flag.
  *     @arg SPI_FLAG_CRCERR: CRC Error flag.
  *     @arg I2S_FLAG_UDR: Underrun Error flag.
  *     @arg I2S_FLAG_CHSIDE: Channel Side flag.
  * @retval The new state of SPI_I2S_FLAG (SET or RESET).
  */
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, unsigned short SPI_I2S_FLAG)
{
  FlagStatus bitstatus = RESET;

  /* Check the status of the specified SPI/I2S flag */
  if ((SPIx->SR & SPI_I2S_FLAG) != (unsigned short)RESET)
  {
    /* SPI_I2S_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* SPI_I2S_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the SPI_I2S_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Clears the SPIx CRC Error (CRCERR) flag.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  * @param  SPI_I2S_FLAG: specifies the SPI flag to clear. 
  *   This function clears only CRCERR flag.
  * @note
  *   - OVR (OverRun error) flag is cleared by software sequence: a read 
  *     operation to SPI_DR register (SPI_I2S_ReceiveData()) followed by a read 
  *     operation to SPI_SR register (SPI_I2S_GetFlagStatus()).
  *   - UDR (UnderRun error) flag is cleared by a read operation to 
  *     SPI_SR register (SPI_I2S_GetFlagStatus()).
  *   - MODF (Mode Fault) flag is cleared by software sequence: a read/write 
  *     operation to SPI_SR register (SPI_I2S_GetFlagStatus()) followed by a 
  *     write operation to SPI_CR1 register (SPI_Cmd() to enable the SPI).
  * @retval None
  */
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, unsigned short SPI_I2S_FLAG)
{
  /* Clear the selected SPI CRC Error (CRCERR) flag */
  SPIx->SR = (unsigned short)~SPI_I2S_FLAG;
}

/**
  * @brief  Checks whether the specified SPI/I2S interrupt has occurred or not.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  *   - 2 or 3 in I2S mode
  * @param  SPI_I2S_IT: specifies the SPI/I2S interrupt source to check. 
  *   This parameter can be one of the following values:
  *     @arg SPI_I2S_IT_TXE: Transmit buffer empty interrupt.
  *     @arg SPI_I2S_IT_RXNE: Receive buffer not empty interrupt.
  *     @arg SPI_I2S_IT_OVR: Overrun interrupt.
  *     @arg SPI_IT_MODF: Mode Fault interrupt.
  *     @arg SPI_IT_CRCERR: CRC Error interrupt.
  *     @arg I2S_IT_UDR: Underrun Error interrupt.
  * @retval The new state of SPI_I2S_IT (SET or RESET).
  */
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, unsigned char SPI_I2S_IT)
{
  ITStatus bitstatus = RESET;
  unsigned short itpos = 0, itmask = 0, enablestatus = 0;

  /* Get the SPI/I2S IT index */
  itpos = 0x01 << (SPI_I2S_IT & 0x0F);

  /* Get the SPI/I2S IT mask */
  itmask = SPI_I2S_IT >> 4;

  /* Set the IT mask */
  itmask = 0x01 << itmask;

  /* Get the SPI_I2S_IT enable bit status */
  enablestatus = (SPIx->CR2 & itmask) ;

  /* Check the status of the specified SPI/I2S interrupt */
  if (((SPIx->SR & itpos) != (unsigned short)RESET) && enablestatus)
  {
    /* SPI_I2S_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* SPI_I2S_IT is reset */
    bitstatus = RESET;
  }
  /* Return the SPI_I2S_IT status */
  return bitstatus;
}

/**
  * @brief  Clears the SPIx CRC Error (CRCERR) interrupt pending bit.
  * @param  SPIx: where x can be
  *   - 1, 2 or 3 in SPI mode 
  * @param  SPI_I2S_IT: specifies the SPI interrupt pending bit to clear.
  *   This function clears only CRCERR intetrrupt pending bit.   
  * @note
  *   - OVR (OverRun Error) interrupt pending bit is cleared by software 
  *     sequence: a read operation to SPI_DR register (SPI_I2S_ReceiveData()) 
  *     followed by a read operation to SPI_SR register (SPI_I2S_GetITStatus()).
  *   - UDR (UnderRun Error) interrupt pending bit is cleared by a read 
  *     operation to SPI_SR register (SPI_I2S_GetITStatus()).
  *   - MODF (Mode Fault) interrupt pending bit is cleared by software sequence:
  *     a read/write operation to SPI_SR register (SPI_I2S_GetITStatus()) 
  *     followed by a write operation to SPI_CR1 register (SPI_Cmd() to enable 
  *     the SPI).
  * @retval None
  */
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, unsigned char SPI_I2S_IT)
{
  unsigned short itpos = 0;

  /* Get the SPI IT index */
  itpos = 0x01 << (SPI_I2S_IT & 0x0F);

  /* Clear the selected SPI CRC Error (CRCERR) interrupt pending bit */
  SPIx->SR = (unsigned short)~itpos;
}

void SPI_ClockEnable(SPI_TypeDef* SPIx, int enable)
{
  if (enable)
  {
    if (SPIx == SPI1)
    {
  	  /* Enable SPI clock, SPI1 */
      RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    }
    else if (SPIx == SPI2)
    {
  	  /* Enable SPI clock, SPI2 */
      RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    }
    else if (SPIx == SPI3)
    {
  	  /* Enable SPI clock, SPI3 */
      RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
    }
  }
  else
  {
    if (SPIx == SPI1)
    {
  	  /* Disable SPI clock, SPI1 */
      RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
    }
    else if (SPIx == SPI2)
    {
  	  /* Enable SPI clock, SPI2 */
      RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
    }
    else if (SPIx == SPI3)
    {
  	  /* Enable SPI clock, SPI3 */
      RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN;
    }
  }
}
