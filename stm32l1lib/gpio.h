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
#define GPIO_AF_RTC_50Hz      ((uint8_t)0x00)  /*!< RTC 50/60 Hz Alternate Function mapping */
#define GPIO_AF_MCO           ((uint8_t)0x00)  /*!< MCO Alternate Function mapping */
#define GPIO_AF_RTC_AF1       ((uint8_t)0x00)  /*!< RTC_AF1 Alternate Function mapping */
#define GPIO_AF_WKUP          ((uint8_t)0x00)  /*!< Wakeup (WKUP1, WKUP2 and WKUP3) Alternate Function mapping */
#define GPIO_AF_SWJ           ((uint8_t)0x00)  /*!< SWJ (SW and JTAG) Alternate Function mapping */
#define GPIO_AF_TRACE         ((uint8_t)0x00)  /*!< TRACE Alternate Function mapping */

/** 
  * @brief  AF 1 selection  
  */ 
#define GPIO_AF_TIM2          ((uint8_t)0x01)  /*!< TIM2 Alternate Function mapping */
/** 
  * @brief  AF 2 selection  
  */ 
#define GPIO_AF_TIM3          ((uint8_t)0x02)  /*!< TIM3 Alternate Function mapping */
#define GPIO_AF_TIM4          ((uint8_t)0x02)  /*!< TIM4 Alternate Function mapping */
#define GPIO_AF_TIM5          ((uint8_t)0x02)  /*!< TIM5 Alternate Function mapping */
/** 
  * @brief  AF 3 selection  
  */ 
#define GPIO_AF_TIM9           ((uint8_t)0x03)  /*!< TIM9 Alternate Function mapping */
#define GPIO_AF_TIM10          ((uint8_t)0x03)  /*!< TIM10 Alternate Function mapping */
#define GPIO_AF_TIM11          ((uint8_t)0x03)  /*!< TIM11 Alternate Function mapping */
/** 
  * @brief  AF 4 selection  
  */ 
#define GPIO_AF_I2C1          ((uint8_t)0x04)  /*!< I2C1 Alternate Function mapping */
#define GPIO_AF_I2C2          ((uint8_t)0x04)  /*!< I2C2 Alternate Function mapping */
/** 
  * @brief  AF 5 selection  
  */ 
#define GPIO_AF_SPI1          ((uint8_t)0x05)  /*!< SPI1 Alternate Function mapping */
#define GPIO_AF_SPI2          ((uint8_t)0x05)  /*!< SPI2 Alternate Function mapping */
/** 
  * @brief  AF 6 selection  
  */ 
#define GPIO_AF_SPI3          ((uint8_t)0x06)  /*!< SPI3 Alternate Function mapping */
/** 
  * @brief  AF 7 selection  
  */ 
#define GPIO_AF_USART1        ((uint8_t)0x07)  /*!< USART1 Alternate Function mapping */
#define GPIO_AF_USART2        ((uint8_t)0x07)  /*!< USART2 Alternate Function mapping */
#define GPIO_AF_USART3        ((uint8_t)0x07)  /*!< USART3 Alternate Function mapping */
/** 
  * @brief  AF 8 selection  
  */ 
#define GPIO_AF_UART4         ((uint8_t)0x08)  /*!< UART4 Alternate Function mapping */
#define GPIO_AF_UART5         ((uint8_t)0x08)  /*!< UART5 Alternate Function mapping */
/** 
  * @brief  AF 10 selection  
  */ 
#define GPIO_AF_USB           ((uint8_t)0xA)  /*!< USB Full speed device  Alternate Function mapping */
/** 
  * @brief  AF 11 selection  
  */ 
#define GPIO_AF_LCD           ((uint8_t)0x0B)  /*!< LCD Alternate Function mapping */
/** 
  * @brief  AF 12 selection  
  */ 
#define GPIO_AF_FSMC           ((uint8_t)0x0C)  /*!< FSMC Alternate Function mapping */
#define GPIO_AF_SDIO           ((uint8_t)0x0C)  /*!< SDIO Alternate Function mapping */
/** 
  * @brief  AF 14 selection  
  */ 
#define GPIO_AF_RI            ((uint8_t)0x0E)  /*!< RI Alternate Function mapping */

/** 
  * @brief  AF 15 selection  
  */ 
#define GPIO_AF_EVENTOUT      ((uint8_t)0x0F)  /*!< EVENTOUT Alternate Function mapping */

#define GPIO_PortSourceGPIOA       ((uint8_t)0x00)
#define GPIO_PortSourceGPIOB       ((uint8_t)0x01)
#define GPIO_PortSourceGPIOC       ((uint8_t)0x02)
#define GPIO_PortSourceGPIOD       ((uint8_t)0x03)
#define GPIO_PortSourceGPIOE       ((uint8_t)0x04)
#define GPIO_PortSourceGPIOF       ((uint8_t)0x05)
#define GPIO_PortSourceGPIOG       ((uint8_t)0x06)

typedef enum
{ 
  GPIO_Mode_IN   = 0x00, /*!< GPIO Input Mode */
  GPIO_Mode_OUT  = 0x01, /*!< GPIO Output Mode */
  GPIO_Mode_AF   = 0x02, /*!< GPIO Alternate function Mode */
  GPIO_Mode_AN   = 0x03  /*!< GPIO Analog Mode */
}GPIOMode_TypeDef;

typedef enum
{ GPIO_OType_PP = 0x00,
  GPIO_OType_OD = 0x01
}GPIOOType_TypeDef;

typedef enum
{ 
  GPIO_Speed_400KHz = 0x00, /*!< Very Low Speed */
  GPIO_Speed_2MHz   = 0x01, /*!< Low Speed */
  GPIO_Speed_10MHz  = 0x02, /*!< Medium Speed */
  GPIO_Speed_40MHz  = 0x03  /*!< High Speed */
}GPIOSpeed_TypeDef;

typedef enum
{ GPIO_PuPd_NOPULL = 0x00,
  GPIO_PuPd_UP     = 0x01,
  GPIO_PuPd_DOWN   = 0x02
}GPIOPuPd_TypeDef;

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
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);

#endif
