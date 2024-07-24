#include <board.h>
#include <i2c.h>

#define TIMING_CLEAR_MASK   (0xF0FFFFFFU)  /*!< I2C TIMING clear register Mask */

/**
  * @brief  Initializes the I2C according to the specified parameters
  *         in the I2C_InitTypeDef and initialize the associated handle.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
void I2C_Init(I2C_TypeDef *instance, I2C_InitTypeDef *init)
{
  /* Configure I2Cx: Frequency range */
  instance->TIMINGR = init->Timing & TIMING_CLEAR_MASK;

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Disable Own Address1 before set the Own Address1 configuration */
  instance->OAR1 &= ~I2C_OAR1_OA1EN;

  /* Configure I2Cx: Own Address1 and ack own address1 mode */
  if (init->AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    instance->OAR1 = (I2C_OAR1_OA1EN | init->OwnAddress1);
  }
  else /* I2C_ADDRESSINGMODE_10BIT */
  {
    instance->OAR1 = (I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | init->OwnAddress1);
    /*---------------------------- I2Cx CR2 Configuration ----------------------*/
    instance->CR2 = (I2C_CR2_ADD10);
  }

  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
  instance->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Disable Own Address2 before set the Own Address2 configuration */
  instance->OAR2 &= ~I2C_DUALADDRESS_ENABLE;

  /* Configure I2Cx: Dual mode and Own Address2 */
  instance->OAR2 = (init->DualAddressMode | init->OwnAddress2 | (init->OwnAddress2Masks << 8));

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  instance->CR1 = (init->GeneralCallMode | init->NoStretchMode);

  /* Enable the selected I2C peripheral */
  instance->CR1 |= I2C_CR1_PE;
}
