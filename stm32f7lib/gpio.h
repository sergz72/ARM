#ifndef _GPIO_H
#define _GPIO_H

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

#define  GPIO_MODE_INPUT                        ((uint32_t)0x00000000U)   /*!< Input Floating Mode                   */
#define  GPIO_MODE_OUTPUT_PP                    ((uint32_t)0x00000001U)   /*!< Output Push Pull Mode                 */
#define  GPIO_MODE_OUTPUT_OD                    ((uint32_t)0x00000011U)   /*!< Output Open Drain Mode                */
#define  GPIO_MODE_AF_PP                        ((uint32_t)0x00000002U)   /*!< Alternate Function Push Pull Mode     */
#define  GPIO_MODE_AF_OD                        ((uint32_t)0x00000012U)   /*!< Alternate Function Open Drain Mode    */

#define  GPIO_MODE_ANALOG                       ((uint32_t)0x00000003U)   /*!< Analog Mode  */
    
#define  GPIO_MODE_IT_RISING                    ((uint32_t)0x10110000U)   /*!< External Interrupt Mode with Rising edge trigger detection          */
#define  GPIO_MODE_IT_FALLING                   ((uint32_t)0x10210000U)   /*!< External Interrupt Mode with Falling edge trigger detection         */
#define  GPIO_MODE_IT_RISING_FALLING            ((uint32_t)0x10310000U)   /*!< External Interrupt Mode with Rising/Falling edge trigger detection  */
 
#define  GPIO_MODE_EVT_RISING                   ((uint32_t)0x10120000U)   /*!< External Event Mode with Rising edge trigger detection               */
#define  GPIO_MODE_EVT_FALLING                  ((uint32_t)0x10220000U)   /*!< External Event Mode with Falling edge trigger detection              */
#define  GPIO_MODE_EVT_RISING_FALLING           ((uint32_t)0x10320000U)   /*!< External Event Mode with Rising/Falling edge trigger detection       */

#define  GPIO_SPEED_FREQ_LOW         ((uint32_t)0x00000000U)  /*!< Low speed     */
#define  GPIO_SPEED_FREQ_MEDIUM      ((uint32_t)0x00000001U)  /*!< Medium speed  */
#define  GPIO_SPEED_FREQ_HIGH        ((uint32_t)0x00000002U)  /*!< Fast speed    */
#define  GPIO_SPEED_FREQ_VERY_HIGH   ((uint32_t)0x00000003U)  /*!< High speed    */

#define  GPIO_NOPULL        ((uint32_t)0x00000000U)   /*!< No Pull-up or Pull-down activation  */
#define  GPIO_PULLUP        ((uint32_t)0x00000001U)   /*!< Pull-up activation                  */
#define  GPIO_PULLDOWN      ((uint32_t)0x00000002U)   /*!< Pull-down activation                */

/** 
  * @brief   AF 0 selection  
  */ 
#define GPIO_AF0_RTC_50Hz      ((uint8_t)0x00U)  /* RTC_50Hz Alternate Function mapping                       */
#define GPIO_AF0_MCO           ((uint8_t)0x00U)  /* MCO (MCO1 and MCO2) Alternate Function mapping            */
#define GPIO_AF0_SWJ           ((uint8_t)0x00U)  /* SWJ (SWD and JTAG) Alternate Function mapping             */
#define GPIO_AF0_TRACE         ((uint8_t)0x00U)  /* TRACE Alternate Function mapping                          */

/** 
  * @brief   AF 1 selection  
  */ 
#define GPIO_AF1_TIM1          ((uint8_t)0x01U)  /* TIM1 Alternate Function mapping */
#define GPIO_AF1_TIM2          ((uint8_t)0x01U)  /* TIM2 Alternate Function mapping */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF1_UART5         ((uint8_t)0x01U)  /* UART5 Alternate Function mapping */
#define GPIO_AF1_I2C4          ((uint8_t)0x01U)  /* I2C4 Alternate Function mapping  */   
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */

/** 
  * @brief   AF 2 selection  
  */ 
#define GPIO_AF2_TIM3          ((uint8_t)0x02U)  /* TIM3 Alternate Function mapping */
#define GPIO_AF2_TIM4          ((uint8_t)0x02U)  /* TIM4 Alternate Function mapping */
#define GPIO_AF2_TIM5          ((uint8_t)0x02U)  /* TIM5 Alternate Function mapping */

/** 
  * @brief   AF 3 selection  
  */ 
#define GPIO_AF3_TIM8          ((uint8_t)0x03U)  /* TIM8 Alternate Function mapping  */
#define GPIO_AF3_TIM9          ((uint8_t)0x03U)  /* TIM9 Alternate Function mapping  */
#define GPIO_AF3_TIM10         ((uint8_t)0x03U)  /* TIM10 Alternate Function mapping */
#define GPIO_AF3_TIM11         ((uint8_t)0x03U)  /* TIM11 Alternate Function mapping */
#define GPIO_AF3_LPTIM1        ((uint8_t)0x03U)  /* LPTIM1 Alternate Function mapping */
#define GPIO_AF3_CEC           ((uint8_t)0x03U)  /* CEC Alternate Function mapping */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF3_DFSDM1         ((uint8_t)0x03U)  /* DFSDM1 Alternate Function mapping */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/** 
  * @brief   AF 4 selection  
  */ 
#define GPIO_AF4_I2C1          ((uint8_t)0x04U)  /* I2C1 Alternate Function mapping */
#define GPIO_AF4_I2C2          ((uint8_t)0x04U)  /* I2C2 Alternate Function mapping */
#define GPIO_AF4_I2C3          ((uint8_t)0x04U)  /* I2C3 Alternate Function mapping */
#define GPIO_AF4_I2C4          ((uint8_t)0x04U)  /* I2C4 Alternate Function mapping */
#define GPIO_AF4_CEC           ((uint8_t)0x04U)  /* CEC Alternate Function mapping */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF4_USART1        ((uint8_t)0x04)  /* USART1 Alternate Function mapping */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */   

/** 
  * @brief   AF 5 selection  
  */ 
#define GPIO_AF5_SPI1          ((uint8_t)0x05U)  /* SPI1 Alternate Function mapping        */
#define GPIO_AF5_SPI2          ((uint8_t)0x05U)  /* SPI2/I2S2 Alternate Function mapping   */
#define GPIO_AF5_SPI3          ((uint8_t)0x05U)  /* SPI3/I2S3 Alternate Function mapping   */
#define GPIO_AF5_SPI4          ((uint8_t)0x05U)  /* SPI4 Alternate Function mapping        */
#define GPIO_AF5_SPI5          ((uint8_t)0x05U)  /* SPI5 Alternate Function mapping        */
#define GPIO_AF5_SPI6          ((uint8_t)0x05U)  /* SPI6 Alternate Function mapping        */

/** 
  * @brief   AF 6 selection  
  */ 
#define GPIO_AF6_SPI3          ((uint8_t)0x06U)  /* SPI3/I2S3 Alternate Function mapping  */
#define GPIO_AF6_SAI1          ((uint8_t)0x06U)  /* SAI1 Alternate Function mapping       */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF6_UART4         ((uint8_t)0x06U)   /* UART4 Alternate Function mapping     */   
#define GPIO_AF6_DFSDM1        ((uint8_t)0x06U)  /* DFSDM1 Alternate Function mapping     */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */   

/** 
  * @brief   AF 7 selection  
  */ 
#define GPIO_AF7_USART1        ((uint8_t)0x07U)  /* USART1 Alternate Function mapping     */
#define GPIO_AF7_USART2        ((uint8_t)0x07U)  /* USART2 Alternate Function mapping     */
#define GPIO_AF7_USART3        ((uint8_t)0x07U)  /* USART3 Alternate Function mapping     */
#define GPIO_AF7_UART5         ((uint8_t)0x07U)  /* UART5 Alternate Function mapping      */
#define GPIO_AF7_SPDIFRX       ((uint8_t)0x07U)  /* SPDIF-RX Alternate Function mapping   */
#define GPIO_AF7_SPI2          ((uint8_t)0x07U)  /* SPI2 Alternate Function mapping       */
#define GPIO_AF7_SPI3          ((uint8_t)0x07U)  /* SPI3 Alternate Function mapping       */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF7_SPI6          ((uint8_t)0x07U)  /* SPI6 Alternate Function mapping       */
#define GPIO_AF7_DFSDM1         ((uint8_t)0x07U) /* DFSDM1 Alternate Function mapping      */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */  

/** 
  * @brief   AF 8 selection  
  */ 
#define GPIO_AF8_UART4         ((uint8_t)0x08U)  /* UART4 Alternate Function mapping  */
#define GPIO_AF8_UART5         ((uint8_t)0x08U)  /* UART5 Alternate Function mapping  */
#define GPIO_AF8_USART6        ((uint8_t)0x08U)  /* USART6 Alternate Function mapping */
#define GPIO_AF8_UART7         ((uint8_t)0x08U)  /* UART7 Alternate Function mapping  */
#define GPIO_AF8_UART8         ((uint8_t)0x08U)  /* UART8 Alternate Function mapping  */
#define GPIO_AF8_SPDIFRX       ((uint8_t)0x08U)  /* SPIDIF-RX Alternate Function mapping */
#define GPIO_AF8_SAI2          ((uint8_t)0x08U)  /* SAI2 Alternate Function mapping   */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF8_SPI6          ((uint8_t)0x08U)  /* SPI6 Alternate Function mapping   */  
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */    


/** 
  * @brief   AF 9 selection 
  */ 
#define GPIO_AF9_CAN1          ((uint8_t)0x09U)  /* CAN1 Alternate Function mapping    */
#define GPIO_AF9_CAN2          ((uint8_t)0x09U)  /* CAN2 Alternate Function mapping    */
#define GPIO_AF9_TIM12         ((uint8_t)0x09U)  /* TIM12 Alternate Function mapping   */
#define GPIO_AF9_TIM13         ((uint8_t)0x09U)  /* TIM13 Alternate Function mapping   */
#define GPIO_AF9_TIM14         ((uint8_t)0x09U)  /* TIM14 Alternate Function mapping   */
#define GPIO_AF9_QUADSPI       ((uint8_t)0x09U)  /* QUADSPI Alternate Function mapping */
#if defined(STM32F746xx) || defined(STM32F756xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF9_LTDC          ((uint8_t)0x09U)  /* LCD-TFT Alternate Function mapping */
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
#if defined(STM32F746xx) || defined(STM32F756xx) || defined(STM32F765xx) || defined(STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF9_FMC           ((uint8_t)0x09U)   /* FMC Alternate Function mapping     */
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/** 
  * @brief   AF 10 selection  
  */ 
#define GPIO_AF10_OTG_FS        ((uint8_t)0xAU)  /* OTG_FS Alternate Function mapping */
#define GPIO_AF10_OTG_HS        ((uint8_t)0xAU)  /* OTG_HS Alternate Function mapping */
#define GPIO_AF10_QUADSPI       ((uint8_t)0xAU)  /* QUADSPI Alternate Function mapping */
#define GPIO_AF10_SAI2          ((uint8_t)0xAU)  /* SAI2 Alternate Function mapping */
#if defined (STM32F765xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF10_DFSDM1         ((uint8_t)0x0AU)  /* DFSDM1 Alternate Function mapping  */
#define GPIO_AF10_SDMMC2        ((uint8_t)0x0AU)  /* SDMMC2 Alternate Function mapping */   
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */   

/** 
  * @brief   AF 11 selection  
  */ 
#define GPIO_AF11_ETH           ((uint8_t)0x0BU)  /* ETHERNET Alternate Function mapping */
#if defined(STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)
#define GPIO_AF11_CAN3          ((uint8_t)0x0BU)  /* CAN3 Alternate Function mapping     */
#define GPIO_AF11_SDMMC2        ((uint8_t)0x0BU)  /* SDMMC2 Alternate Function mapping   */
#define GPIO_AF11_I2C4          ((uint8_t)0x0BU)  /* I2C4 Alternate Function mapping     */   
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
   
/** 
  * @brief   AF 12 selection  
  */ 
#define GPIO_AF12_FMC           ((uint8_t)0xCU)  /* FMC Alternate Function mapping                      */
#define GPIO_AF12_OTG_HS_FS     ((uint8_t)0xCU)  /* OTG HS configured in FS, Alternate Function mapping */
#define GPIO_AF12_SDMMC1        ((uint8_t)0xCU)  /* SDMMC1 Alternate Function mapping                   */
#if defined(STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) || defined (STM32F779xx)   
#define GPIO_AF12_MDIOS        ((uint8_t)0xCU)  /* SDMMC1 Alternate Function mapping                    */
#define GPIO_AF12_UART7        ((uint8_t)0xCU)  /* UART7 Alternate Function mapping                     */   
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
   
/** 
  * @brief   AF 13 selection  
  */ 
#define GPIO_AF13_DCMI          ((uint8_t)0x0DU)  /* DCMI Alternate Function mapping */
#if defined (STM32F769xx) || defined (STM32F779xx)   
#define GPIO_AF13_DSI           ((uint8_t)0x0DU)  /* DSI Alternate Function mapping  */
#endif /* STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */   
#if defined(STM32F746xx) || defined(STM32F756xx) || defined(STM32F767xx) || defined(STM32F769xx) || defined(STM32F777xx) || defined(STM32F779xx)
#define GPIO_AF13_LTDC          ((uint8_t)0x0DU)  /* LTDC Alternate Function mapping */   
   
/** 
  * @brief   AF 14 selection  
  */
#define GPIO_AF14_LTDC          ((uint8_t)0x0EU)  /* LCD-TFT Alternate Function mapping */
#endif /* STM32F746xx || STM32F756xx || STM32F767xx || STM32F769xx || STM32F777xx || STM32F779xx */
/** 
  * @brief   AF 15 selection  
  */ 
#define GPIO_AF15_EVENTOUT      ((uint8_t)0x0FU)  /* EVENTOUT Alternate Function mapping */

typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
}GPIO_PinState;

typedef struct
{
  uint32_t Pin;       /*!< Specifies the GPIO pins to be configured.
                           This parameter can be any value of @ref GPIO_pins_define */

  uint32_t Mode;      /*!< Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref GPIO_mode_define */

  uint32_t Pull;      /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                           This parameter can be a value of @ref GPIO_pull_define */

  uint32_t Speed;     /*!< Specifies the speed for the selected pins.
                           This parameter can be a value of @ref GPIO_speed_define */

  uint32_t Alternate;  /*!< Peripheral to be connected to the selected pins. 
                            This parameter can be a value of @ref GPIO_Alternate_function_selection */
}GPIO_InitTypeDef;

void GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);
void GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

#endif
