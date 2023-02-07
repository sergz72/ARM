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
  uint32_t pinpos = 0x00, pos = 0x00 , currentpin = 0x00;

  /* -------------------------Configure the port pins---------------- */
  /*-- GPIO Mode Configuration --*/
  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
  {
    pos = ((uint32_t)0x01) << pinpos;
    /* Get the port pins position */
    currentpin = GPIO_Pin & pos;

    if (currentpin == pos)
    {
      GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2));
      GPIOx->MODER |= ((uint32_t)GPIO_Mode) << (pinpos * 2);

      if ((GPIO_Mode == GPIO_Mode_OUT) || (GPIO_Mode == GPIO_Mode_AF))
      {
        /* Speed mode configuration */
        GPIOx->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (pinpos * 2));
        GPIOx->OSPEEDR |= (uint32_t)GPIO_Speed << (pinpos * 2);

        /* Output mode configuration*/
        GPIOx->OTYPER  &= ~((GPIO_OTYPER_OT_0) << ((uint16_t)pinpos)) ;
        GPIOx->OTYPER |= (uint16_t)((uint16_t)GPIO_OType << (uint16_t)pinpos);
      }

      /* Pull-up Pull down resistor configuration*/
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (uint16_t)pinpos * 2);
      GPIOx->PUPDR |= ((uint32_t)GPIO_PuPd << (pinpos * 2));
    }
  }
}

void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
  uint32_t temp = 0x00;
  uint32_t temp_2 = 0x00;
  
  temp = ((uint32_t)(GPIO_AF) << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4)) ;
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((uint32_t)0xF << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4)) ;
  temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
  GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
}

void GPIO_EXTILineConfig(unsigned char GPIO_PortSource, unsigned char GPIO_PinSource)
{
  unsigned int tmp;

  tmp = ((unsigned int)0x0F) << (0x04 * (GPIO_PinSource & (unsigned char)0x03));
  SYSCFG->EXTICR[GPIO_PinSource >> 0x02] &= ~tmp;
  SYSCFG->EXTICR[GPIO_PinSource >> 0x02] |= (((unsigned int)GPIO_PortSource) << (0x04 * (GPIO_PinSource & (unsigned char)0x03)));
}
