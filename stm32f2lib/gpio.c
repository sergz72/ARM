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
        GPIOx->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinpos)) ;
        GPIOx->OTYPER |= (uint16_t)((uint16_t)GPIO_OType << (uint16_t)pinpos);
      }

      /* Pull-up Pull down resistor configuration*/
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (uint16_t)pinpos * 2);
      GPIOx->PUPDR |= ((unsigned int)GPIO_PuPd << (pinpos * 2));
    }
  }
}

void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
  unsigned int temp;
  unsigned int temp_2;
  
  temp = ((unsigned int)(GPIO_AF) << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4)) ;
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((unsigned int)0xF << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4)) ;
  temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
  GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
}

/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  * @param  GPIO_PortSource: selects the GPIO port to be used as source for EXTI lines.
  *   This parameter can be GPIO_PortSourceGPIOx where x can be (A..G).
  * @param  GPIO_PinSource: specifies the EXTI line to be configured.
  *   This parameter can be GPIO_PinSourcex where x can be (0..15).
  * @retval None
  */
void GPIO_EXTILineConfig(unsigned int GPIO_PortSource, unsigned int GPIO_PinSource)
{
  unsigned int tmp;

  tmp = 0x0FU << 4U * (GPIO_PinSource & 3U);
  SYSCFG->EXTICR[GPIO_PinSource >> 2U] &= ~tmp;
  SYSCFG->EXTICR[GPIO_PinSource >> 2U] |= GPIO_PortSource << (4U * (GPIO_PinSource & 3U));
}
