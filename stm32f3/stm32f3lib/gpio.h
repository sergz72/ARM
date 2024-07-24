#ifndef _GPIO_H
#define _GPIO_H

#define GPIO_PinSource0            ((uint8_t)0x00)
#define GPIO_PinSource1            ((uint8_t)0x01)
#define GPIO_PinSource2            ((uint8_t)0x02)
#define GPIO_PinSource3            ((uint8_t)0x03)
#define GPIO_PinSource4            ((uint8_t)0x04)
#define GPIO_PinSource5            ((uint8_t)0x05)
#define GPIO_PinSource6            ((uint8_t)0x06)
#define GPIO_PinSource7            ((uint8_t)0x07)
#define GPIO_PinSource8            ((uint8_t)0x08)
#define GPIO_PinSource9            ((uint8_t)0x09)
#define GPIO_PinSource10           ((uint8_t)0x0A)
#define GPIO_PinSource11           ((uint8_t)0x0B)
#define GPIO_PinSource12           ((uint8_t)0x0C)
#define GPIO_PinSource13           ((uint8_t)0x0D)
#define GPIO_PinSource14           ((uint8_t)0x0E)
#define GPIO_PinSource15           ((uint8_t)0x0F)

#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /* All pins selected */

/** 
  * @brief  AF 0 selection
  */ 
#define GPIO_AF_0            ((uint8_t)0x00) /* JTCK-SWCLK, JTDI, JTDO/TRACESW0, JTMS-SWDAT,  
                                                MCO, NJTRST, TRACED, TRACECK */
/** 
  * @brief  AF 1 selection
  */ 
#define GPIO_AF_1            ((uint8_t)0x01) /*  OUT, TIM2, TIM15, TIM16, TIM17 */

/** 
  * @brief  AF 2 selection
  */ 
#define GPIO_AF_2            ((uint8_t)0x02) /* COMP1_OUT, TIM1, TIM2, TIM3, TIM4, TIM8, TIM15, TIM16 */

/** 
  * @brief  AF 3 selection
  */ 
#define GPIO_AF_3            ((uint8_t)0x03) /* COMP7_OUT, TIM8, TIM15, Touch, HRTIM1 */

/** 
  * @brief  AF 4 selection
  */ 
#define GPIO_AF_4            ((uint8_t)0x04) /* I2C1, I2C2, TIM1, TIM8, TIM16, TIM17 */

/** 
  * @brief  AF 5 selection
  */ 
#define GPIO_AF_5            ((uint8_t)0x05) /* IR_OUT, I2S2, I2S3, SPI1, SPI2, TIM8, USART4, USART5 */

/** 
  * @brief  AF 6 selection
  */ 
#define GPIO_AF_6            ((uint8_t)0x06) /*  IR_OUT, I2S2, I2S3, SPI2, SPI3, TIM1, TIM8 */

/** 
  * @brief  AF 7 selection
  */ 
#define GPIO_AF_7            ((uint8_t)0x07) /* AOP2_OUT, CAN, COMP3_OUT, COMP5_OUT, COMP6_OUT, 
                                                USART1, USART2, USART3 */

/** 
  * @brief  AF 8 selection
  */ 
#define GPIO_AF_8            ((uint8_t)0x08) /* COMP1_OUT, COMP2_OUT, COMP3_OUT, COMP4_OUT, 
                                                COMP5_OUT, COMP6_OUT */

/** 
  * @brief  AF 9 selection
  */ 
#define GPIO_AF_9            ((uint8_t)0x09) /* AOP4_OUT, CAN, TIM1, TIM8, TIM15 */

/** 
  * @brief  AF 10 selection
  */ 
#define GPIO_AF_10            ((uint8_t)0x0A) /* AOP1_OUT, AOP3_OUT, TIM2, TIM3, TIM4, TIM8, TIM17 */

/** 
  * @brief  AF 11 selection
  */ 
#define GPIO_AF_11            ((uint8_t)0x0B) /* TIM1, TIM8 */

/** 
   * @brief  AF 12 selection
   */ 
#define GPIO_AF_12            ((uint8_t)0x0C) /* TIM1, HRTIM1 */

/** 
   * @brief  AF 13 selection
   */ 
#define GPIO_AF_13            ((uint8_t)0x0D) /* HRTIM1, AOP2_OUT */

/** 
  * @brief  AF 14 selection
  */ 
#define GPIO_AF_14            ((uint8_t)0x0E) /* USBDM, USBDP */

/** 
  * @brief  AF 15 selection
  */ 
#define GPIO_AF_15            ((uint8_t)0x0F) /* OUT */

#define GPIO_Speed_10MHz GPIO_Speed_Level_1   /*!< Fast Speed:10MHz   */
#define GPIO_Speed_2MHz  GPIO_Speed_Level_2   /*!< Medium Speed:2MHz  */
#define GPIO_Speed_50MHz GPIO_Speed_Level_3   /*!< High Speed:50MHz   */

#define GPIO_PortSourceGPIOA       ((uint8_t)0x00)
#define GPIO_PortSourceGPIOB       ((uint8_t)0x01)
#define GPIO_PortSourceGPIOC       ((uint8_t)0x02)
#define GPIO_PortSourceGPIOD       ((uint8_t)0x03)
#define GPIO_PortSourceGPIOE       ((uint8_t)0x04)
#define GPIO_PortSourceGPIOF       ((uint8_t)0x05)
#define GPIO_PortSourceGPIOG       ((uint8_t)0x06)

typedef enum
{ 
  GPIO_Speed_Level_1  = 0x01, /*!< Fast Speed     */
  GPIO_Speed_Level_2  = 0x02, /*!< Meduim Speed   */
  GPIO_Speed_Level_3  = 0x03  /*!< High Speed     */
} GPIOSpeed_TypeDef;

typedef enum
{ 
  GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode */
  GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode */
  GPIO_Mode_AF   = 0x02, /*!< GPIO Alternate function Mode */
  GPIO_Mode_AN   = 0x03  /*!< GPIO Analog Mode */
} GPIOMode_TypeDef;

typedef enum
{ 
  GPIO_OType_PP = 0x00,
  GPIO_OType_OD = 0x01
} GPIOOType_TypeDef;

typedef enum
{ 
  GPIO_PuPd_NOPULL = 0x00,
  GPIO_PuPd_UP     = 0x01,
  GPIO_PuPd_DOWN   = 0x02
} GPIOPuPd_TypeDef;

typedef enum
{ 
  Bit_RESET = 0,
  Bit_SET
} BitAction;

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
);

/* GPIO Alternate functions configuration function ****************************/
void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF);
void GPIO_EXTILineConfig(unsigned char GPIO_PortSource, unsigned char GPIO_PinSource);

#endif
