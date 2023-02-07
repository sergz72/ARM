/**
  ******************************************************************************
  * @file    stm32f10x_exti.h
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    11-March-2011
  * @brief   This file contains all the functions prototypes for the EXTI firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_EXTI_H
#define __STM32F10x_EXTI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <board.h>

/** @addtogroup STM32F10x_StdPeriph_Driver
  * @{
  */

/** @addtogroup EXTI
  * @{
  */

/** @defgroup EXTI_Exported_Types
  * @{
  */

/** 
  * @brief  EXTI mode enumeration  
  */

typedef enum
{
  EXTI_Mode_Interrupt = 0x00,
  EXTI_Mode_Event = 0x04
}EXTIMode_TypeDef;

/** 
  * @brief  EXTI Trigger enumeration  
  */

typedef enum
{
  EXTI_Trigger_Rising = 0x08,
  EXTI_Trigger_Falling = 0x0C,  
  EXTI_Trigger_Rising_Falling = 0x10
}EXTITrigger_TypeDef;

/**
  * @}
  */

/** @defgroup EXTI_Exported_Constants
  * @{
  */

/** @defgroup EXTI_Lines 
  * @{
  */

#define EXTI_Line0       ((uint32_t)0x00001)  /*!< External interrupt line 0 */
#define EXTI_Line1       ((uint32_t)0x00002)  /*!< External interrupt line 1 */
#define EXTI_Line2       ((uint32_t)0x00004)  /*!< External interrupt line 2 */
#define EXTI_Line3       ((uint32_t)0x00008)  /*!< External interrupt line 3 */
#define EXTI_Line4       ((uint32_t)0x00010)  /*!< External interrupt line 4 */
#define EXTI_Line5       ((uint32_t)0x00020)  /*!< External interrupt line 5 */
#define EXTI_Line6       ((uint32_t)0x00040)  /*!< External interrupt line 6 */
#define EXTI_Line7       ((uint32_t)0x00080)  /*!< External interrupt line 7 */
#define EXTI_Line8       ((uint32_t)0x00100)  /*!< External interrupt line 8 */
#define EXTI_Line9       ((uint32_t)0x00200)  /*!< External interrupt line 9 */
#define EXTI_Line10      ((uint32_t)0x00400)  /*!< External interrupt line 10 */
#define EXTI_Line11      ((uint32_t)0x00800)  /*!< External interrupt line 11 */
#define EXTI_Line12      ((uint32_t)0x01000)  /*!< External interrupt line 12 */
#define EXTI_Line13      ((uint32_t)0x02000)  /*!< External interrupt line 13 */
#define EXTI_Line14      ((uint32_t)0x04000)  /*!< External interrupt line 14 */
#define EXTI_Line15      ((uint32_t)0x08000)  /*!< External interrupt line 15 */

#if defined (EXTI_IMR_MR16)
#define EXTI_LINE_16                       (EXTI_CONFIG      | 0x10u)    /*!< External interrupt line 16 Connected to the PVD Output */
#else
#define EXTI_LINE_16                       (EXTI_RESERVED    | 0x10u)
#endif /* EXTI_IMR_MR16 */

#define EXTI_LINE_17                       (EXTI_CONFIG      | 0x11u)    /*!< External interrupt line 17 Connected to the RTC Alarm event */

#if defined (EXTI_IMR_MR18)
#define EXTI_LINE_18                       (EXTI_CONFIG      | 0x12u)    /*!< External interrupt line 18 Connected to the USB OTG FS Wakeup from suspend event */
#else
#define EXTI_LINE_18                       (EXTI_RESERVED    | 0x12u)
#endif /* EXTI_IMR_MR18 */

#define EXTI_LINE_19                       (EXTI_CONFIG      | 0x13u)    /*!< External interrupt line 19 Connected to the Ethernet Wakeup event */

#if defined (EXTI_IMR_MR20)
#define EXTI_LINE_20                       (EXTI_CONFIG      | 0x14u)    /*!< External interrupt line 20 Connected to the USB OTG HS (configured in FS) Wakeup event  */
#else
#define EXTI_LINE_20                       (EXTI_RESERVED    | 0x14u)
#endif /* EXTI_IMR_MR20 */

#if defined (EXTI_IMR_MR21)
#define EXTI_LINE_21                       (EXTI_CONFIG      | 0x15u)    /*!< External interrupt line 21 Connected to the Comparator 1 output */
#else
#define EXTI_LINE_21                       (EXTI_RESERVED    | 0x15u)
#endif /* EXTI_IMR_MR21 */

#if defined (EXTI_IMR_MR22)
#define EXTI_LINE_22                       (EXTI_CONFIG      | 0x16u)    /*!< External interrupt line 22 Connected to the Comparator 2 output */
#else
#define EXTI_LINE_22                       (EXTI_RESERVED    | 0x16u)
#endif /* EXTI_IMR_MR22 */

#if defined (EXTI_IMR_MR23)
#define EXTI_LINE_23                       (EXTI_DIRECT      | 0x17u)    /*!< External interrupt line 23 Connected to the internal I2C1 wakeup event  */
#else
#define EXTI_LINE_23                       (EXTI_RESERVED    | 0x17u)
#endif /* EXTI_IMR_MR23 */

#define EXTI_LINE_24                       (EXTI_RESERVED    | 0x18u)

#if defined (EXTI_IMR_MR25)
#define EXTI_LINE_25                       (EXTI_CONFIG      | 0x19u)    /*!< External interrupt line 25 Connected to the internal USART1 wakeup event  */
#else
#define EXTI_LINE_25                       (EXTI_RESERVED    | 0x19u)
#endif /* EXTI_IMR_MR25 */

#if defined (EXTI_IMR_MR26)
#define EXTI_LINE_26                       (EXTI_CONFIG      | 0x1Au)    /*!< External interrupt line 26 Connected to the internal USART2 wakeup event  */
#else
#define EXTI_LINE_26                       (EXTI_RESERVED    | 0x1Au)
#endif /* EXTI_IMR_MR26 */

#if defined (EXTI_IMR_MR27)
#define EXTI_LINE_27                       (EXTI_CONFIG      | 0x1Bu)    /*!< External interrupt line 27 Connected to the internal CEC wakeup event  */
#else
#define EXTI_LINE_27                       (EXTI_RESERVED    | 0x1Bu)
#endif /* EXTI_IMR_MR27 */

#if defined (EXTI_IMR_MR28)
#define EXTI_LINE_28                       (EXTI_CONFIG      | 0x1Cu)    /*!< External interrupt line 28 Connected to the  internal USART3 wakeup event   */
#else
#define EXTI_LINE_28                       (EXTI_RESERVED    | 0x1Cu)
#endif /* EXTI_IMR_MR28 */

#define EXTI_LINE_29                       (EXTI_RESERVED    | 0x1Du)
#define EXTI_LINE_30                       (EXTI_RESERVED    | 0x1Eu)

#if defined (EXTI_IMR_MR31)
#define EXTI_LINE_31                       (EXTI_CONFIG      | 0x1Fu)    /*!< External interrupt line 31 Connected to the VDDIO2 supply comparator output  */
#else
#define EXTI_LINE_31                       (EXTI_RESERVED    | 0x1Fu)
#endif /* EXTI_IMR_MR31 */
                                          
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup EXTI_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup EXTI_Exported_Functions
  * @{
  */

void EXTI_DeInit(void);
void EXTI_Init(
  uint32_t EXTI_Line,               /*!< Specifies the EXTI lines to be enabled or disabled.
                                         This parameter can be any combination of @ref EXTI_Lines */
   
  EXTIMode_TypeDef EXTI_Mode,       /*!< Specifies the mode for the EXTI lines.
                                         This parameter can be a value of @ref EXTIMode_TypeDef */

  EXTITrigger_TypeDef EXTI_Trigger, /*!< Specifies the trigger signal active edge for the EXTI lines.
                                         This parameter can be a value of @ref EXTIMode_TypeDef */

  FunctionalState EXTI_LineCmd      /*!< Specifies the new state of the selected EXTI lines.
                                         This parameter can be set either to ENABLE or DISABLE */ 
);
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);
void EXTI_ClearFlag(uint32_t EXTI_Line);
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
void EXTI_ClearITPendingBit(uint32_t EXTI_Line);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F10x_EXTI_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
