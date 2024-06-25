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

/**
  * @brief  Writes data to the specified GPIO data port.
  * @param  GPIOx: where x can be (A, B, C, D, E or F) to select the GPIO peripheral.
  * @note   GPIOC, GPIOD, GPIOE and GPIOF  are available only for STM32F072 and STM32F091. 
  * @param  GPIO_PinSource: specifies the pin for the Alternate function.
  *          This parameter can be GPIO_PinSourcex where x can be (0..15) for GPIOA, GPIOB, GPIOD, GPIOE
  *          and (0..12) for GPIOC and (0, 2..5, 9..10) for GPIOF.    
  * @param  GPIO_AF: selects the pin to used as Alternate function.
  *          This parameter can be one of the following value:
  *            @arg GPIO_AF_0:  WKUP, EVENTOUT, TIM15, SPI1, TIM17, MCO, SWDAT, SWCLK,
  *                             TIM14, BOOT, USART1, CEC, IR_OUT, SPI2, TIM3, USART4,
  *                             CAN, USART2, CRS, TIM16, TIM1, TS, USART8 
  *            @arg GPIO_AF_1: USART2, CEC, TIM3, USART1, USART2, EVENTOUT, I2C1,
  *                            I2C2, TIM15, SPI2, USART3, TS, SPI1, USART7, USART8
  *                            USART5, USART4, USART6, I2C1   
  *            @arg GPIO_AF_2: TIM2, TIM1, EVENTOUT, TIM16, TIM17, USB, USART6, USART5,
  *                            USART8, USART7, USART6  
  *            @arg GPIO_AF_3: TS, I2C1, TIM15, EVENTOUT 
  *            @arg GPIO_AF_4: TIM14, USART4, USART3, CRS, CAN, I2C1, USART5
  *            @arg GPIO_AF_5: TIM16, TIM17, TIM15, SPI2, I2C2, USART6, MCO
  *            @arg GPIO_AF_6: EVENTOUT
  *            @arg GPIO_AF_7: COMP1 OUT, COMP2 OUT 
  * @note   The pin should already been configured in Alternate Function mode(AF)
  *         using GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
  * @note   Refer to the Alternate function mapping table in the device datasheet 
  *         for the detailed mapping of the system and peripherals'alternate 
  *         function I/O pins.
  * @retval None
  */
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, unsigned short GPIO_PinSource, unsigned char GPIO_AF)
{
  unsigned int temp = 0x00;
  unsigned int temp_2 = 0x00;

  temp = ((unsigned int)(GPIO_AF) << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4));
  GPIOx->AFR[GPIO_PinSource >> 0x03] &= ~((unsigned int)0xF << ((unsigned int)((unsigned int)GPIO_PinSource & (unsigned int)0x07) * 4));
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
