#ifndef _DMA2D_H
#define _DMA2D_H

/* Exported types ------------------------------------------------------------*/
/** @defgroup DMA2D_Exported_Types DMA2D Exported Types
  * @{
  */
#define MAX_DMA2D_LAYER  2

/** 
  * @brief DMA2D color Structure definition
  */
typedef struct
{
  unsigned int Blue;               /*!< Configures the blue value.
                                    This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  unsigned int Green;              /*!< Configures the green value.
                                    This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  unsigned int Red;                /*!< Configures the red value.
                                    This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */
} DMA2D_ColorTypeDef;

/** 
  * @brief DMA2D CLUT Structure definition
  */
typedef struct
{
  unsigned int *pCLUT;                  /*!< Configures the DMA2D CLUT memory address.*/

  unsigned int CLUTColorMode;           /*!< Configures the DMA2D CLUT color mode.
                                         This parameter can be one value of @ref DMA2D_CLUT_CM. */

  unsigned int Size;                    /*!< Configures the DMA2D CLUT size. 
                                         This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF.*/
} DMA2D_CLUTCfgTypeDef;

/** 
  * @brief DMA2D Init structure definition
  */
typedef struct
{
  unsigned int             Mode;               /*!< Configures the DMA2D transfer mode.
                                                This parameter can be one value of @ref DMA2D_Mode. */

  unsigned int             ColorMode;          /*!< Configures the color format of the output image.
                                                This parameter can be one value of @ref DMA2D_Output_Color_Mode. */

  unsigned int             OutputOffset;       /*!< Specifies the Offset value. 
                                                This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x3FFF. */
#if defined (DMA2D_OPFCCR_AI)
  unsigned int             AlphaInverted;     /*!< Select regular or inverted alpha value for the output pixel format converter.
                                               This parameter can be one value of @ref DMA2D_Alpha_Inverted. */
#endif /* DMA2D_OPFCCR_AI */  

#if defined (DMA2D_OPFCCR_RBS) 
  unsigned int             RedBlueSwap;       /*!< Select regular mode (RGB or ARGB) or swap mode (BGR or ABGR)
                                               for the output pixel format converter.
                                               This parameter can be one value of @ref DMA2D_RB_Swap. */ 
#endif /* DMA2D_OPFCCR_RBS */
  
} DMA2D_InitTypeDef;


/** 
  * @brief DMA2D Layer structure definition
  */
typedef struct
{
  unsigned int             InputOffset;       /*!< Configures the DMA2D foreground or background offset.
                                               This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x3FFF. */

  unsigned int             InputColorMode;    /*!< Configures the DMA2D foreground or background color mode. 
                                               This parameter can be one value of @ref DMA2D_Input_Color_Mode. */

  unsigned int             AlphaMode;         /*!< Configures the DMA2D foreground or background alpha mode. 
                                               This parameter can be one value of @ref DMA2D_Alpha_Mode. */

  unsigned int             InputAlpha;        /*!< Specifies the DMA2D foreground or background alpha value and color value in case of A8 or A4 color mode. 
                                               This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF except for the color modes detailed below.
                                               @note In case of A8 or A4 color mode (ARGB), this parameter must be a number between 
                                               Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF where 
                                               - InputAlpha[24:31] is the alpha value ALPHA[0:7]
                                               - InputAlpha[16:23] is the red value RED[0:7]
                                               - InputAlpha[8:15] is the green value GREEN[0:7]
                                               - InputAlpha[0:7] is the blue value BLUE[0:7]. */

#if defined (DMA2D_FGPFCCR_AI) && defined (DMA2D_BGPFCCR_AI) 
  unsigned int             AlphaInverted;     /*!< Select regular or inverted alpha value.
                                               This parameter can be one value of @ref DMA2D_Alpha_Inverted. 
                                               This feature is only available on devices :
                                               STM32F756xx, STM32F767xx, STM32F769xx, STM32F777xx and STM32F779xx.*/
  
#endif /* (DMA2D_FGPFCCR_AI) && (DMA2D_BGPFCCR_AI)  */   

#if defined (DMA2D_FGPFCCR_RBS) && defined (DMA2D_BGPFCCR_RBS)   
  unsigned int             RedBlueSwap;       /*!< Select regular mode (RGB or ARGB) or swap mode (BGR or ABGR).
                                               This parameter can be one value of @ref DMA2D_RB_Swap
                                               This feature is only available on devices :
                                               STM32F756xx, STM32F767xx, STM32F769xx, STM32F777xx and STM32F779xx.*/  

#endif /* (DMA2D_FGPFCCR_RBS) && (DMA2D_BGPFCCR_RBS)  */
  
} DMA2D_LayerCfgTypeDef;

/** 
  * @brief  HAL DMA2D State structures definition
  */
typedef enum
{
  DMA2D_STATE_RESET             = 0x00U,    /*!< DMA2D not yet initialized or disabled       */
  DMA2D_STATE_READY             = 0x01U,    /*!< Peripheral Initialized and ready for use    */
  DMA2D_STATE_BUSY              = 0x02U,    /*!< An internal process is ongoing              */
  DMA2D_STATE_TIMEOUT           = 0x03U,    /*!< Timeout state                               */
  DMA2D_STATE_ERROR             = 0x04U,    /*!< DMA2D state error                           */
  DMA2D_STATE_SUSPEND           = 0x05U     /*!< DMA2D process is suspended                  */
} DMA2D_StateTypeDef;

/** 
  * @brief  DMA2D handle Structure definition
  */
typedef struct __DMA2D_HandleTypeDef
{
  DMA2D_TypeDef               *Instance;                                                    /*!< DMA2D register base address.               */
                                                                                                                                          
  DMA2D_InitTypeDef           Init;                                                         /*!< DMA2D communication parameters.            */

  void                        (* XferCpltCallback)(struct __DMA2D_HandleTypeDef * hdma2d);  /*!< DMA2D transfer complete callback.          */
                                                                                                                                           
  void                        (* XferErrorCallback)(struct __DMA2D_HandleTypeDef * hdma2d); /*!< DMA2D transfer error callback.             */                                                                                                                                             

  DMA2D_LayerCfgTypeDef       LayerCfg[MAX_DMA2D_LAYER];                                    /*!< DMA2D Layers parameters           */  

  __IO DMA2D_StateTypeDef     State;                                                        /*!< DMA2D transfer state.                      */
                                                                                                                                           
  __IO unsigned int               ErrorCode;                                                    /*!< DMA2D error code.                          */  
} DMA2D_HandleTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup DMA2D_Exported_Constants DMA2D Exported Constants
  * @{
  */

/** @defgroup DMA2D_Error_Code DMA2D Error Code
  * @{
  */
#define DMA2D_ERROR_NONE        ((unsigned int)0x00000000U)  /*!< No error             */
#define DMA2D_ERROR_TE          ((unsigned int)0x00000001U)  /*!< Transfer error       */
#define DMA2D_ERROR_CE          ((unsigned int)0x00000002U)  /*!< Configuration error  */
#define DMA2D_ERROR_CAE         ((unsigned int)0x00000004U)  /*!< CLUT access error    */
#define DMA2D_ERROR_TIMEOUT     ((unsigned int)0x00000020U)  /*!< Timeout error        */
/**
  * @}
  */

/** @defgroup DMA2D_Mode DMA2D Mode 
  * @{
  */
#define DMA2D_M2M                   ((unsigned int)0x00000000U)  /*!< DMA2D memory to memory transfer mode */
#define DMA2D_M2M_PFC               DMA2D_CR_MODE_0          /*!< DMA2D memory to memory with pixel format conversion transfer mode */
#define DMA2D_M2M_BLEND             DMA2D_CR_MODE_1          /*!< DMA2D memory to memory with blending transfer mode */
#define DMA2D_R2M                   DMA2D_CR_MODE            /*!< DMA2D register to memory transfer mode */
/**
  * @}
  */

/** @defgroup DMA2D_Output_Color_Mode DMA2D Output Color Mode 
  * @{
  */
#define DMA2D_OUTPUT_ARGB8888       ((unsigned int)0x00000000U)               /*!< ARGB8888 DMA2D color mode */
#define DMA2D_OUTPUT_RGB888         DMA2D_OPFCCR_CM_0                     /*!< RGB888 DMA2D color mode   */
#define DMA2D_OUTPUT_RGB565         DMA2D_OPFCCR_CM_1                     /*!< RGB565 DMA2D color mode   */
#define DMA2D_OUTPUT_ARGB1555       (DMA2D_OPFCCR_CM_0|DMA2D_OPFCCR_CM_1) /*!< ARGB1555 DMA2D color mode */
#define DMA2D_OUTPUT_ARGB4444       DMA2D_OPFCCR_CM_2                     /*!< ARGB4444 DMA2D color mode */
/**
  * @}
  */

/** @defgroup DMA2D_Input_Color_Mode DMA2D Input Color Mode
  * @{
  */
#define DMA2D_INPUT_ARGB8888        ((unsigned int)0x00000000U)  /*!< ARGB8888 color mode */
#define DMA2D_INPUT_RGB888          ((unsigned int)0x00000001U)  /*!< RGB888 color mode   */
#define DMA2D_INPUT_RGB565          ((unsigned int)0x00000002U)  /*!< RGB565 color mode   */
#define DMA2D_INPUT_ARGB1555        ((unsigned int)0x00000003U)  /*!< ARGB1555 color mode */
#define DMA2D_INPUT_ARGB4444        ((unsigned int)0x00000004U)  /*!< ARGB4444 color mode */
#define DMA2D_INPUT_L8              ((unsigned int)0x00000005U)  /*!< L8 color mode       */
#define DMA2D_INPUT_AL44            ((unsigned int)0x00000006U)  /*!< AL44 color mode     */
#define DMA2D_INPUT_AL88            ((unsigned int)0x00000007U)  /*!< AL88 color mode     */
#define DMA2D_INPUT_L4              ((unsigned int)0x00000008U)  /*!< L4 color mode       */
#define DMA2D_INPUT_A8              ((unsigned int)0x00000009U)  /*!< A8 color mode       */
#define DMA2D_INPUT_A4              ((unsigned int)0x0000000AU)  /*!< A4 color mode       */
/**
  * @}
  */

/** @defgroup DMA2D_Alpha_Mode DMA2D Alpha Mode
  * @{
  */
#define DMA2D_NO_MODIF_ALPHA        ((unsigned int)0x00000000U)  /*!< No modification of the alpha channel value */
#define DMA2D_REPLACE_ALPHA         ((unsigned int)0x00000001U)  /*!< Replace original alpha channel value by programmed alpha value */
#define DMA2D_COMBINE_ALPHA         ((unsigned int)0x00000002U)  /*!< Replace original alpha channel value by programmed alpha value
                                                                with original alpha channel value                              */
/**
  * @}
  */    

#if defined (DMA2D_FGPFCCR_AI) && defined (DMA2D_BGPFCCR_AI)    
/** @defgroup DMA2D_Alpha_Inverted DMA2D ALPHA Inversion
  * @{
  */
#define DMA2D_REGULAR_ALPHA         ((unsigned int)0x00000000U)  /*!< No modification of the alpha channel value */
#define DMA2D_INVERTED_ALPHA        ((unsigned int)0x00000001U)  /*!< Invert the alpha channel value */                                  
/**
  * @}
  */
#endif /* (DMA2D_FGPFCCR_AI) && (DMA2D_BGPFCCR_AI)  */

#if defined (DMA2D_FGPFCCR_RBS) && defined (DMA2D_BGPFCCR_RBS)  
/** @defgroup DMA2D_RB_Swap DMA2D Red and Blue Swap
  * @{
  */
#define DMA2D_RB_REGULAR            ((unsigned int)0x00000000U)  /*!< Select regular mode (RGB or ARGB) */
#define DMA2D_RB_SWAP               ((unsigned int)0x00000001U)  /*!< Select swap mode (BGR or ABGR) */
/**
  * @}
  */ 
#endif /* (DMA2D_FGPFCCR_RBS) && (DMA2D_BGPFCCR_RBS)  */     

/** @defgroup DMA2D_CLUT_CM DMA2D CLUT Color Mode
  * @{
  */
#define DMA2D_CCM_ARGB8888          ((unsigned int)0x00000000U)  /*!< ARGB8888 DMA2D CLUT color mode */
#define DMA2D_CCM_RGB888            ((unsigned int)0x00000001U)  /*!< RGB888 DMA2D CLUT color mode   */
/**
  * @}
  */


/** @defgroup DMA2D_Interrupts DMA2D Interrupts 
  * @{
  */
#define DMA2D_IT_CE                 DMA2D_CR_CEIE            /*!< Configuration Error Interrupt */
#define DMA2D_IT_CTC                DMA2D_CR_CTCIE           /*!< CLUT Transfer Complete Interrupt */
#define DMA2D_IT_CAE                DMA2D_CR_CAEIE           /*!< CLUT Access Error Interrupt */
#define DMA2D_IT_TW                 DMA2D_CR_TWIE            /*!< Transfer Watermark Interrupt */
#define DMA2D_IT_TC                 DMA2D_CR_TCIE            /*!< Transfer Complete Interrupt */
#define DMA2D_IT_TE                 DMA2D_CR_TEIE            /*!< Transfer Error Interrupt */
/**                                                         
  * @}                                                      
  */                                                        
                                                            
/** @defgroup DMA2D_Flags DMA2D Flags                       
  * @{                                                      
  */                                                        
#define DMA2D_FLAG_CE               DMA2D_ISR_CEIF           /*!< Configuration Error Interrupt Flag */
#define DMA2D_FLAG_CTC              DMA2D_ISR_CTCIF          /*!< CLUT Transfer Complete Interrupt Flag */
#define DMA2D_FLAG_CAE              DMA2D_ISR_CAEIF          /*!< CLUT Access Error Interrupt Flag */
#define DMA2D_FLAG_TW               DMA2D_ISR_TWIF           /*!< Transfer Watermark Interrupt Flag */
#define DMA2D_FLAG_TC               DMA2D_ISR_TCIF           /*!< Transfer Complete Interrupt Flag */
#define DMA2D_FLAG_TE               DMA2D_ISR_TEIF           /*!< Transfer Error Interrupt Flag */
/**
  * @}
  */
  
/** @defgroup DMA2D_Aliases DMA2D API Aliases
  * @{
  */
#define DMA2D_DisableCLUT       DMA2D_CLUTLoading_Abort    /*!< Aliased to DMA2D_CLUTLoading_Abort for compatibility with legacy code */
/**
  * @}
  */
  
  
/**
  * @}
  */
/* Exported macros ------------------------------------------------------------*/
/** @defgroup DMA2D_Exported_Macros DMA2D Exported Macros
  * @{
  */

/** @brief Reset DMA2D handle state
  * @param  __HANDLE__: specifies the DMA2D handle.
  * @retval None
  */
#define __DMA2D_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = DMA2D_STATE_RESET)

/**
  * @brief  Enable the DMA2D.
  * @param  __HANDLE__: DMA2D handle
  * @retval None.
  */
#define __DMA2D_ENABLE(__HANDLE__)        ((__HANDLE__)->Instance->CR |= DMA2D_CR_START)


/* Interrupt & Flag management */
/**
  * @brief  Get the DMA2D pending flags.
  * @param  __HANDLE__: DMA2D handle
  * @param  __FLAG__: flag to check.
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_FLAG_CE:  Configuration error flag
  *            @arg DMA2D_FLAG_CTC: CLUT transfer complete flag
  *            @arg DMA2D_FLAG_CAE: CLUT access error flag
  *            @arg DMA2D_FLAG_TW:  Transfer Watermark flag
  *            @arg DMA2D_FLAG_TC:  Transfer complete flag
  *            @arg DMA2D_FLAG_TE:  Transfer error flag   
  * @retval The state of FLAG.
  */
#define __DMA2D_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ISR & (__FLAG__))

/**
  * @brief  Clear the DMA2D pending flags.
  * @param  __HANDLE__: DMA2D handle
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_FLAG_CE:  Configuration error flag
  *            @arg DMA2D_FLAG_CTC: CLUT transfer complete flag
  *            @arg DMA2D_FLAG_CAE: CLUT access error flag
  *            @arg DMA2D_FLAG_TW:  Transfer Watermark flag
  *            @arg DMA2D_FLAG_TC:  Transfer complete flag
  *            @arg DMA2D_FLAG_TE:  Transfer error flag    
  * @retval None
  */
#define __DMA2D_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->IFCR = (__FLAG__))

/**
  * @brief  Enable the specified DMA2D interrupts.
  * @param  __HANDLE__: DMA2D handle
  * @param __INTERRUPT__: specifies the DMA2D interrupt sources to be enabled. 
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_IT_CE:  Configuration error interrupt mask
  *            @arg DMA2D_IT_CTC: CLUT transfer complete interrupt mask
  *            @arg DMA2D_IT_CAE: CLUT access error interrupt mask
  *            @arg DMA2D_IT_TW:  Transfer Watermark interrupt mask
  *            @arg DMA2D_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA2D_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __DMA2D_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR |= (__INTERRUPT__))

/**
  * @brief  Disable the specified DMA2D interrupts.
  * @param  __HANDLE__: DMA2D handle
  * @param __INTERRUPT__: specifies the DMA2D interrupt sources to be disabled. 
  *          This parameter can be any combination of the following values:
  *            @arg DMA2D_IT_CE:  Configuration error interrupt mask
  *            @arg DMA2D_IT_CTC: CLUT transfer complete interrupt mask
  *            @arg DMA2D_IT_CAE: CLUT access error interrupt mask
  *            @arg DMA2D_IT_TW:  Transfer Watermark interrupt mask
  *            @arg DMA2D_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA2D_IT_TE:  Transfer error interrupt mask
  * @retval None
  */
#define __DMA2D_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR &= ~(__INTERRUPT__))

/**
  * @brief  Check whether the specified DMA2D interrupt source is enabled or not.
  * @param  __HANDLE__: DMA2D handle
  * @param  __INTERRUPT__: specifies the DMA2D interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg DMA2D_IT_CE:  Configuration error interrupt mask
  *            @arg DMA2D_IT_CTC: CLUT transfer complete interrupt mask
  *            @arg DMA2D_IT_CAE: CLUT access error interrupt mask
  *            @arg DMA2D_IT_TW:  Transfer Watermark interrupt mask
  *            @arg DMA2D_IT_TC:  Transfer complete interrupt mask
  *            @arg DMA2D_IT_TE:  Transfer error interrupt mask
  * @retval The state of INTERRUPT source.
  */
#define __DMA2D_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->CR & (__INTERRUPT__))
     
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/  
/** @addtogroup DMA2D_Exported_Functions DMA2D Exported Functions
  * @{
  */

/** @addtogroup DMA2D_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */  
  
/* Initialization and de-initialization functions *******************************/
unsigned int DMA2D_Init(DMA2D_HandleTypeDef *hdma2d);
unsigned int DMA2D_DeInit (DMA2D_HandleTypeDef *hdma2d);
void              DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d);
void              DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d);

/**
  * @}
  */


/** @addtogroup DMA2D_Exported_Functions_Group2 IO operation functions
  * @{
  */
  
/* IO operation functions *******************************************************/
void DMA2D_Start(DMA2D_HandleTypeDef *hdma2d, unsigned int pdata, unsigned int DstAddress, unsigned int Width, unsigned int Height);
void DMA2D_BlendingStart(DMA2D_HandleTypeDef *hdma2d, unsigned int SrcAddress1, unsigned int SrcAddress2, unsigned int DstAddress, unsigned int Width,  unsigned int Height);
void DMA2D_Start_IT(DMA2D_HandleTypeDef *hdma2d, unsigned int pdata, unsigned int DstAddress, unsigned int Width, unsigned int Height);
void DMA2D_BlendingStart_IT(DMA2D_HandleTypeDef *hdma2d, unsigned int SrcAddress1, unsigned int SrcAddress2, unsigned int DstAddress, unsigned int Width, unsigned int Height);
unsigned int DMA2D_Suspend(DMA2D_HandleTypeDef *hdma2d);
void DMA2D_Resume(DMA2D_HandleTypeDef *hdma2d);
unsigned int DMA2D_Abort(DMA2D_HandleTypeDef *hdma2d);
void DMA2D_EnableCLUT(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx);
void DMA2D_CLUTLoad(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, unsigned int LayerIdx);
void DMA2D_CLUTLoad_IT(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, unsigned int LayerIdx);
unsigned int DMA2D_CLUTLoading_Abort(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx);
unsigned int DMA2D_CLUTLoading_Suspend(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx);
void DMA2D_CLUTLoading_Resume(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx);
unsigned int DMA2D_PollForTransfer(DMA2D_HandleTypeDef *hdma2d, unsigned int Timeout);
void              _DMA2D_IRQHandler(DMA2D_HandleTypeDef *hdma2d);
void              DMA2D_LineEventCallback(DMA2D_HandleTypeDef *hdma2d);
void              DMA2D_CLUTLoadingCpltCallback(DMA2D_HandleTypeDef *hdma2d);

/**
  * @}
  */

/** @addtogroup DMA2D_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */

/* Peripheral Control functions *************************************************/
void DMA2D_ConfigLayer(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx);
void DMA2D_ConfigCLUT(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, unsigned int LayerIdx);
unsigned int DMA2D_ProgramLineEvent(DMA2D_HandleTypeDef *hdma2d, unsigned int Line);
void DMA2D_EnableDeadTime(DMA2D_HandleTypeDef *hdma2d);
void DMA2D_DisableDeadTime(DMA2D_HandleTypeDef *hdma2d);
void DMA2D_ConfigDeadTime(DMA2D_HandleTypeDef *hdma2d, uint8_t DeadTime);

/**
  * @}
  */

/** @addtogroup DMA2D_Exported_Functions_Group4 Peripheral State and Error functions
  * @{
  */

/* Peripheral State functions ***************************************************/
DMA2D_StateTypeDef DMA2D_GetState(DMA2D_HandleTypeDef *hdma2d);
unsigned int               DMA2D_GetError(DMA2D_HandleTypeDef *hdma2d);

/**
  * @}
  */

/**
  * @}
  */

/* Private constants ---------------------------------------------------------*/  
  
/** @addtogroup DMA2D_Private_Constants DMA2D Private Constants
  * @{
  */                         

/** @defgroup DMA2D_Maximum_Line_WaterMark DMA2D Maximum Line Watermark 
  * @{
  */
#define DMA2D_LINE_WATERMARK_MAX            DMA2D_LWR_LW       /*!< DMA2D maximum line watermark */
/**
  * @}
  */
  
/** @defgroup DMA2D_Color_Value DMA2D Color Value
  * @{
  */
#define DMA2D_COLOR_VALUE                 ((unsigned int)0x000000FFU)  /*!< Color value mask */
/**
  * @}
  */      

/** @defgroup DMA2D_Max_Layer DMA2D Maximum Number of Layers
  * @{
  */  
#define DMA2D_MAX_LAYER         2         /*!< DMA2D maximum number of layers */  
/**
  * @}
  */ 
  
/** @defgroup DMA2D_Offset DMA2D Offset 
  * @{
  */
#define DMA2D_OFFSET                DMA2D_FGOR_LO            /*!< Line Offset */
/**
  * @}
  */ 
  
/** @defgroup DMA2D_Size DMA2D Size 
  * @{
  */
#define DMA2D_PIXEL                 (DMA2D_NLR_PL >> 16U)    /*!< DMA2D number of pixels per line */
#define DMA2D_LINE                  DMA2D_NLR_NL             /*!< DMA2D number of lines           */
/**
  * @}
  */    
  
/** @defgroup DMA2D_CLUT_Size DMA2D CLUT Size
  * @{
  */
#define DMA2D_CLUT_SIZE             (DMA2D_FGPFCCR_CS >> 8)  /*!< DMA2D CLUT size */

#endif /* _DMA2D_H */
