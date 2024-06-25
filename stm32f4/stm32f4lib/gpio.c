#include <board.h>
#include <gpio.h>

void GPIO_Init(GPIO_TypeDef* GPIOx,
               unsigned int GPIO_Pin,              /*!< Specifies the GPIO pins to be configured.
                                                    This parameter can be any value of @ref GPIO_pins_define */

               GPIOMode_TypeDef GPIO_Mode,     /*!< Specifies the operating mode for the selected pins.
                                                    This parameter can be a value of @ref GPIOMode_TypeDef */

               GPIOSpeed_TypeDef GPIO_Speed,   /*!< Specifies the speed for the selected pins.
                                                    This parameter can be a value of @ref GPIOSpeed_TypeDef */

               GPIOOType_TypeDef GPIO_OType,   /*!< Specifies the operating output type for the selected pins.
                                                    This parameter can be a value of @ref GPIOOType_TypeDef */

               GPIOPuPd_TypeDef GPIO_PuPd      /*!< Specifies the operating Pull-up/Pull down for the selected pins.
                                                    This parameter can be a value of @ref GPIOPuPd_TypeDef */
)
{
  unsigned int pinpos = 0x00, pos = 0x00 , currentpin = 0x00;

  /* -------------------------Configure the port pins---------------- */
  /*-- GPIO Mode Configuration --*/
  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
  {
    pos = ((unsigned int)0x01) << pinpos;
    /* Get the port pins position */
    currentpin = GPIO_Pin & pos;

    if (currentpin == pos)
    {
      GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2));
      GPIOx->MODER |= ((unsigned int)GPIO_Mode) << (pinpos * 2);

      if ((GPIO_Mode == GPIO_Mode_OUT) || (GPIO_Mode == GPIO_Mode_AF))
      {
        /* Speed mode configuration */
        GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
        GPIOx->OSPEEDR |= (unsigned int)GPIO_Speed << (pinpos * 2);

        /* Output mode configuration*/
        GPIOx->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((unsigned short)pinpos)) ;
        GPIOx->OTYPER |= (unsigned short)((unsigned short)GPIO_OType << (unsigned short)pinpos);
      }

      /* Pull-up Pull down resistor configuration*/
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (unsigned short)pinpos * 2);
      GPIOx->PUPDR |= ((unsigned int)GPIO_PuPd << (pinpos * 2));
    }
  }
}

void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, unsigned short GPIO_PinSource, unsigned char GPIO_AF)
{
  unsigned int temp = 0x00;
  unsigned int temp_2 = 0x00;
  
  temp = ((unsigned int)(GPIO_AF) << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4)) ;
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((unsigned int)0xF << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4)) ;
  temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
  GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
}

/**
  * @brief  Sets or clears the selected data port bit.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices. 
  * @param  GPIO_Pin: specifies the port bit to be written.
  *          This parameter can be one of GPIO_Pin_x where x can be (0..15).
  * @param  BitVal: specifies the value to be written to the selected bit.
  *          This parameter can be one of the BitAction enum values:
  *            @arg Bit_RESET: to clear the port pin
  *            @arg Bit_SET: to set the port pin
  * @retval None
  */
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, unsigned short GPIO_Pin, BitAction BitVal)
{
  if (BitVal != Bit_RESET)
  {
    GPIOx->BSRR = GPIO_Pin;
  }
  else
  {
    GPIOx->BSRR = ((unsigned int)GPIO_Pin << 16);
  }
}

/**
  * @brief  Sets the selected data port bits.
  * @note   This functions uses GPIOx_BSRR register to allow atomic read/modify 
  *         accesses. In this way, there is no risk of an IRQ occurring between
  *         the read and the modify access.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices. 
  * @param  GPIO_Pin: specifies the port bits to be written.
  *          This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void GPIO_SetBits(GPIO_TypeDef* GPIOx, unsigned short GPIO_Pin)
{
  GPIOx->BSRR = GPIO_Pin;
}

/**
  * @brief  Clears the selected data port bits.
  * @note   This functions uses GPIOx_BSRR register to allow atomic read/modify 
  *         accesses. In this way, there is no risk of an IRQ occurring between
  *         the read and the modify access.
  * @param  GPIOx: where x can be (A..K) to select the GPIO peripheral for STM32F405xx/407xx and STM32F415xx/417xx devices
  *                      x can be (A..I) to select the GPIO peripheral for STM32F42xxx/43xxx devices.
  *                      x can be (A, B, C, D and H) to select the GPIO peripheral for STM32F401xx devices. 
  * @param  GPIO_Pin: specifies the port bits to be written.
  *          This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
  * @retval None
  */
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, unsigned short GPIO_Pin)
{
  GPIOx->BSRR = ((unsigned int)GPIO_Pin << 16);
}
