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
  unsigned int pinpos, pinpos2, pos = 1, currentpin, tmp;

  /* -------------------------Configure the port pins---------------- */
  /*-- GPIO Mode Configuration --*/
  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
  {
    /* Get the port pins position */
    currentpin = GPIO_Pin & pos;

    if (currentpin)
    {
      pinpos2 = pinpos << 1;
      tmp = GPIOx->MODER & ~(GPIO_MODER_MODE0 << pinpos2);
      GPIOx->MODER = tmp | ((unsigned int)GPIO_Mode << pinpos2);

      if ((GPIO_Mode == GPIO_Mode_OUT) || (GPIO_Mode == GPIO_Mode_AF))
      {
        /* Speed mode configuration */
        tmp = GPIOx->OSPEEDR & ~(GPIO_OSPEEDR_OSPEED0 << pinpos2);
        GPIOx->OSPEEDR = tmp | ((unsigned int)GPIO_Speed << pinpos2);

        /* Output mode configuration*/
        tmp = GPIOx->OTYPER  & ~(GPIO_OTYPER_OT0 << pinpos);
        GPIOx->OTYPER = tmp | ((unsigned int)GPIO_OType << pinpos);
      }

      /* Pull-up Pull down resistor configuration*/
      tmp = GPIOx->PUPDR & ~(GPIO_PUPDR_PUPD0 << pinpos2);
      GPIOx->PUPDR = tmp | ((unsigned int)GPIO_PuPd << pinpos2);
    }
    pos <<= 1;
  }
}

void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
  uint32_t temp;
  uint32_t temp_2;

  temp_2 = ((uint32_t)GPIO_PinSource & 0x07) << 2;
  temp = (uint32_t)GPIO_AF << temp_2;
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((uint32_t)0xF << temp_2);
  temp_2 = GPIOx->AFR[GPIO_PinSource >> 0x03] | temp;
  GPIOx->AFR[GPIO_PinSource >> 0x03] = temp_2;
}
