#ifndef _LTDC_H
#define _LTDC_H

/* Exported types ------------------------------------------------------------*/
/** @defgroup LTDC_Exported_Types LTDC Exported Types
  * @{
  */
#define MAX_LAYER  2

/** 
  * @brief  LTDC color structure definition
  */
typedef struct
{
  unsigned char Blue;                    /*!< Configures the blue value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  unsigned char Green;                   /*!< Configures the green value.
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  unsigned char Red;                     /*!< Configures the red value. 
                                        This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  unsigned char Reserved;                /*!< Reserved 0xFF */
} LTDC_ColorTypeDef;

/** 
  * @brief  LTDC Init structure definition
  */
typedef struct
{
  unsigned int            HSPolarity;                /*!< configures the horizontal synchronization polarity.
                                                      This parameter can be one value of @ref LTDC_HS_POLARITY */

  unsigned int            VSPolarity;                /*!< configures the vertical synchronization polarity.
                                                      This parameter can be one value of @ref LTDC_VS_POLARITY */

  unsigned int            DEPolarity;                /*!< configures the data enable polarity. 
                                                      This parameter can be one of value of @ref LTDC_DE_POLARITY */

  unsigned int            PCPolarity;                /*!< configures the pixel clock polarity. 
                                                      This parameter can be one of value of @ref LTDC_PC_POLARITY */

  unsigned int            HorizontalSync;            /*!< configures the number of Horizontal synchronization width.
                                                      This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */

  unsigned int            VerticalSync;              /*!< configures the number of Vertical synchronization height. 
                                                      This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */

  unsigned int            AccumulatedHBP;            /*!< configures the accumulated horizontal back porch width.
                                                      This parameter must be a number between Min_Data = LTDC_HorizontalSync and Max_Data = 0xFFF. */

  unsigned int            AccumulatedVBP;            /*!< configures the accumulated vertical back porch height.
                                                      This parameter must be a number between Min_Data = LTDC_VerticalSync and Max_Data = 0x7FF. */

  unsigned int            AccumulatedActiveW;        /*!< configures the accumulated active width. 
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedHBP and Max_Data = 0xFFF. */

  unsigned int            AccumulatedActiveH;        /*!< configures the accumulated active height.
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedVBP and Max_Data = 0x7FF. */

  unsigned int            TotalWidth;                /*!< configures the total width.
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedActiveW and Max_Data = 0xFFF. */

  unsigned int            TotalHeigh;                /*!< configures the total height.
                                                      This parameter must be a number between Min_Data = LTDC_AccumulatedActiveH and Max_Data = 0x7FF. */

  LTDC_ColorTypeDef   Backcolor;                 /*!< Configures the background color. */
} LTDC_InitTypeDef;

/** 
  * @brief  LTDC Layer structure definition
  */
typedef struct
{
  unsigned int WindowX0;                   /*!< Configures the Window Horizontal Start Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */

  unsigned int WindowX1;                   /*!< Configures the Window Horizontal Stop Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0xFFF. */

  unsigned int WindowY0;                   /*!< Configures the Window vertical Start Position.
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */

  unsigned int WindowY1;                   /*!< Configures the Window vertical Stop Position.
                                            This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x7FF. */

  unsigned int PixelFormat;                /*!< Specifies the pixel format. 
                                            This parameter can be one of value of @ref LTDC_Pixelformat */

  unsigned int Alpha;                      /*!< Specifies the constant alpha used for blending.
                                            This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  unsigned int Alpha0;                     /*!< Configures the default alpha value.
                                            This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. */

  unsigned int BlendingFactor1;            /*!< Select the blending factor 1. 
                                            This parameter can be one of value of @ref LTDC_BlendingFactor1 */

  unsigned int BlendingFactor2;            /*!< Select the blending factor 2. 
                                            This parameter can be one of value of @ref LTDC_BlendingFactor2 */

  unsigned int FBStartAdress;              /*!< Configures the color frame buffer address */

  unsigned int ImageWidth;                 /*!< Configures the color frame buffer line length. 
                                            This parameter must be a number between Min_Data = 0x0000 and Max_Data = 0x1FFF. */

  unsigned int ImageHeight;                /*!< Specifies the number of line in frame buffer. 
                                            This parameter must be a number between Min_Data = 0x000 and Max_Data = 0x7FF. */

  LTDC_ColorTypeDef   Backcolor;       /*!< Configures the layer background color. */
} LTDC_LayerCfgTypeDef;

/** 
  * @brief  HAL LTDC State structures definition
  */
typedef enum
{
  LTDC_STATE_RESET             = 0x00U,    /*!< LTDC not yet initialized or disabled */
  LTDC_STATE_READY             = 0x01U,    /*!< LTDC initialized and ready for use   */
  LTDC_STATE_BUSY              = 0x02U,    /*!< LTDC internal process is ongoing     */
  LTDC_STATE_TIMEOUT           = 0x03U,    /*!< LTDC Timeout state                   */
  LTDC_STATE_ERROR             = 0x04U     /*!< LTDC state error                     */
}LTDC_StateTypeDef;

/** 
  * @brief  LTDC handle Structure definition
  */
typedef struct
{
  LTDC_TypeDef                *Instance;                /*!< LTDC Register base address                */

  LTDC_InitTypeDef            Init;                     /*!< LTDC parameters                           */

  LTDC_LayerCfgTypeDef        LayerCfg[MAX_LAYER];      /*!< LTDC Layers parameters                    */

  __IO LTDC_StateTypeDef  State;                    /*!< LTDC state                                */

  __IO unsigned int               ErrorCode;                /*!< LTDC Error code                           */

} LTDC_HandleTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup LTDC_Exported_Constants LTDC Exported Constants
  * @{
  */

/** @defgroup LTDC_Error_Code LTDC Error Code
  * @{
  */
#define LTDC_ERROR_NONE      ((unsigned int)0x00000000U)    /*!< LTDC No error             */
#define LTDC_ERROR_TE        ((unsigned int)0x00000001U)    /*!< LTDC Transfer error       */
#define LTDC_ERROR_FU        ((unsigned int)0x00000002U)    /*!< LTDC FIFO Underrun        */
#define LTDC_ERROR_TIMEOUT   ((unsigned int)0x00000020U)    /*!< LTDC Timeout error        */
/**
  * @}
  */

/** @defgroup LTDC_HS_POLARITY LTDC HS POLARITY
  * @{
  */
#define LTDC_HSPOLARITY_AL                ((unsigned int)0x00000000U)                /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPOLARITY_AH                LTDC_GCR_HSPOL                        /*!< Horizontal Synchronization is active high. */
/**
  * @}
  */

/** @defgroup LTDC_VS_POLARITY LTDC VS POLARITY
  * @{
  */
#define LTDC_VSPOLARITY_AL                ((unsigned int)0x00000000U)                /*!< Vertical Synchronization is active low. */
#define LTDC_VSPOLARITY_AH                LTDC_GCR_VSPOL                        /*!< Vertical Synchronization is active high. */
/**
  * @}
  */
  
/** @defgroup LTDC_DE_POLARITY LTDC DE POLARITY
  * @{
  */
#define LTDC_DEPOLARITY_AL                ((unsigned int)0x00000000U)                /*!< Data Enable, is active low. */
#define LTDC_DEPOLARITY_AH                LTDC_GCR_DEPOL                        /*!< Data Enable, is active high. */
/**
  * @}
  */

/** @defgroup LTDC_PC_POLARITY LTDC PC POLARITY
  * @{
  */
#define LTDC_PCPOLARITY_IPC               ((unsigned int)0x00000000U)                /*!< input pixel clock. */
#define LTDC_PCPOLARITY_IIPC              LTDC_GCR_PCPOL                        /*!< inverted input pixel clock. */
/**
  * @}
  */

/** @defgroup LTDC_SYNC LTDC SYNC
  * @{
  */
#define LTDC_HORIZONTALSYNC               (LTDC_SSCR_HSW >> 16)                 /*!< Horizontal synchronization width. */ 
#define LTDC_VERTICALSYNC                 LTDC_SSCR_VSH                         /*!< Vertical synchronization height. */
/**
  * @}
  */

/** @defgroup LTDC_BACK_COLOR LTDC BACK COLOR
  * @{
  */
#define LTDC_COLOR                   ((unsigned int)0x000000FFU)                     /*!< Color mask */ 
/**
  * @}
  */
      
/** @defgroup LTDC_BlendingFactor1 LTDC Blending Factor1
  * @{
  */
#define LTDC_BLENDING_FACTOR1_CA                       ((unsigned int)0x00000400U)   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR1_PAxCA                    ((unsigned int)0x00000600U)   /*!< Blending factor : Cte Alpha x Pixel Alpha*/
/**
  * @}
  */

/** @defgroup LTDC_BlendingFactor2 LTDC Blending Factor2
  * @{
  */
#define LTDC_BLENDING_FACTOR2_CA                       ((unsigned int)0x00000005U)   /*!< Blending factor : Cte Alpha */
#define LTDC_BLENDING_FACTOR2_PAxCA                    ((unsigned int)0x00000007U)   /*!< Blending factor : Cte Alpha x Pixel Alpha*/
/**
  * @}
  */
      
/** @defgroup LTDC_Pixelformat LTDC Pixel format
  * @{
  */
#define LTDC_PIXEL_FORMAT_ARGB8888                  ((unsigned int)0x00000000U)      /*!< ARGB8888 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_RGB888                    ((unsigned int)0x00000001U)      /*!< RGB888 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_RGB565                    ((unsigned int)0x00000002U)      /*!< RGB565 LTDC pixel format   */
#define LTDC_PIXEL_FORMAT_ARGB1555                  ((unsigned int)0x00000003U)      /*!< ARGB1555 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_ARGB4444                  ((unsigned int)0x00000004U)      /*!< ARGB4444 LTDC pixel format */
#define LTDC_PIXEL_FORMAT_L8                        ((unsigned int)0x00000005U)      /*!< L8 LTDC pixel format       */
#define LTDC_PIXEL_FORMAT_AL44                      ((unsigned int)0x00000006U)      /*!< AL44 LTDC pixel format     */
#define LTDC_PIXEL_FORMAT_AL88                      ((unsigned int)0x00000007U)      /*!< AL88 LTDC pixel format     */
/**
  * @}
  */

/** @defgroup LTDC_Alpha LTDC Alpha
  * @{
  */
#define LTDC_ALPHA               LTDC_LxCACR_CONSTA                             /*!< LTDC Cte Alpha mask */
/**
  * @}
  */

/** @defgroup LTDC_LAYER_Config LTDC LAYER Config
  * @{
  */
#define LTDC_STOPPOSITION                 (LTDC_LxWHPCR_WHSPPOS >> 16)          /*!< LTDC Layer stop position  */
#define LTDC_STARTPOSITION                LTDC_LxWHPCR_WHSTPOS                  /*!< LTDC Layer start position */

#define LTDC_COLOR_FRAME_BUFFER           LTDC_LxCFBLR_CFBLL                    /*!< LTDC Layer Line length    */ 
#define LTDC_LINE_NUMBER                  LTDC_LxCFBLNR_CFBLNBR                 /*!< LTDC Layer Line number    */
/**
  * @}
  */

/** @defgroup LTDC_Interrupts LTDC Interrupts
  * @{
  */
#define LTDC_IT_LI                      LTDC_IER_LIE
#define LTDC_IT_FU                      LTDC_IER_FUIE
#define LTDC_IT_TE                      LTDC_IER_TERRIE
#define LTDC_IT_RR                      LTDC_IER_RRIE
/**
  * @}
  */
      
/** @defgroup LTDC_Flag LTDC Flag
  * @{
  */
#define LTDC_FLAG_LI                     LTDC_ISR_LIF
#define LTDC_FLAG_FU                     LTDC_ISR_FUIF
#define LTDC_FLAG_TE                     LTDC_ISR_TERRIF
#define LTDC_FLAG_RR                     LTDC_ISR_RRIF
/**
  * @}
  */

/** @defgroup LTDC_Reload_Type LTDC Reload Type
  * @{
  */
#define LTDC_RELOAD_IMMEDIATE            LTDC_SRCR_IMR       /*!< Immediate Reload */
#define LTDC_RELOAD_VERTICAL_BLANKING    LTDC_SRCR_VBR       /*!< Vertical Blanking Reload */
/**
  * @}
  */

/**
  * @}
  */  

/* Exported macro ------------------------------------------------------------*/
/** @defgroup LTDC_Exported_Macros LTDC Exported Macros
  * @{
  */

/** @brief Reset LTDC handle state
  * @param  __HANDLE__: specifies the LTDC handle.
  * @retval None
  */
#define __LTDC_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = LTDC_STATE_RESET)

/**
  * @brief  Enable the LTDC.
  * @param  __HANDLE__: LTDC handle
  * @retval None.
  */
#define __LTDC_ENABLE(__HANDLE__)    ((__HANDLE__)->Instance->GCR |= LTDC_GCR_LTDCEN)

/**
  * @brief  Disable the LTDC.
  * @param  __HANDLE__: LTDC handle
  * @retval None.
  */
#define __LTDC_DISABLE(__HANDLE__)   ((__HANDLE__)->Instance->GCR &= ~(LTDC_GCR_LTDCEN))

/**
  * @brief  Enable the LTDC Layer.
  * @param  __HANDLE__: LTDC handle
  * @param  __LAYER__: Specify the layer to be enabled
  *                     This parameter can be 0 or 1
  * @retval None.
  */
#define __LTDC_LAYER_ENABLE(__HANDLE__, __LAYER__)  ((LTDC_LAYER((__HANDLE__), (__LAYER__)))->CR |= (unsigned int)LTDC_LxCR_LEN)

/**
  * @brief  Disable the LTDC Layer.
  * @param  __HANDLE__: LTDC handle
  * @param  __LAYER__: Specify the layer to be disabled
  *                     This parameter can be 0 or 1
  * @retval None.
  */
#define __LTDC_LAYER_DISABLE(__HANDLE__, __LAYER__) ((LTDC_LAYER((__HANDLE__), (__LAYER__)))->CR &= ~(unsigned int)LTDC_LxCR_LEN)

/**
  * @brief  Reload  Layer Configuration.
  * @param  __HANDLE__: LTDC handle
  * @retval None.
  */
#define __LTDC_RELOAD_CONFIG(__HANDLE__)   ((__HANDLE__)->Instance->SRCR |= LTDC_SRCR_IMR)

/* Interrupt & Flag management */
/**
  * @brief  Get the LTDC pending flags.
  * @param  __HANDLE__: LTDC handle
  * @param  __FLAG__: Get the specified flag.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_FLAG_LI: Line Interrupt flag 
  *            @arg LTDC_FLAG_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_FLAG_TE: Transfer Error interrupt flag
  *            @arg LTDC_FLAG_RR: Register Reload Interrupt Flag 
  * @retval The state of FLAG (SET or RESET).
  */
#define __LTDC_GET_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ISR & (__FLAG__))

/**
  * @brief  Clears the LTDC pending flags.
  * @param  __HANDLE__: LTDC handle
  * @param  __FLAG__: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_FLAG_LI: Line Interrupt flag 
  *            @arg LTDC_FLAG_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_FLAG_TE: Transfer Error interrupt flag
  *            @arg LTDC_FLAG_RR: Register Reload Interrupt Flag 
  * @retval None
  */
#define __LTDC_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->ICR = (__FLAG__))

/**
  * @brief  Enables the specified LTDC interrupts.
  * @param  __HANDLE__: LTDC handle
  * @param __INTERRUPT__: specifies the LTDC interrupt sources to be enabled. 
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag 
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  * @retval None
  */
#define __LTDC_ENABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER |= (__INTERRUPT__))

/**
  * @brief  Disables the specified LTDC interrupts.
  * @param  __HANDLE__: LTDC handle
  * @param __INTERRUPT__: specifies the LTDC interrupt sources to be disabled. 
  *          This parameter can be any combination of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag 
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  * @retval None
  */
#define __LTDC_DISABLE_IT(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->IER &= ~(__INTERRUPT__))

/**
  * @brief  Checks whether the specified LTDC interrupt has occurred or not.
  * @param  __HANDLE__: LTDC handle
  * @param  __INTERRUPT__: specifies the LTDC interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg LTDC_IT_LI: Line Interrupt flag 
  *            @arg LTDC_IT_FU: FIFO Underrun Interrupt flag
  *            @arg LTDC_IT_TE: Transfer Error interrupt flag
  *            @arg LTDC_IT_RR: Register Reload Interrupt Flag
  * @retval The state of INTERRUPT (SET or RESET).
  */
#define __LTDC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__) ((__HANDLE__)->Instance->ISR & (__INTERRUPT__))
/**
  * @}
  */

#if defined (STM32F769xx) || defined (STM32F779xx)  
void LTDC_StructInitFromVideoConfig(LTDC_HandleTypeDef* hltdc, DSI_VidCfgTypeDef *VidCfg);
void LTDC_StructInitFromAdaptedCommandConfig(LTDC_HandleTypeDef* hltdc, DSI_CmdCfgTypeDef *CmdCfg);
#endif

/** @addtogroup LTDC_Exported_Functions_Group1
  * @{
  */
/* Initialization and de-initialization functions *****************************/
unsigned int LTDC_Init(LTDC_HandleTypeDef *hltdc);
void LTDC_DeInit(LTDC_HandleTypeDef *hltdc);
void LTDC_MspInit(LTDC_HandleTypeDef* hltdc);
void LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc);
void LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc);
void LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc);
void LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc);
/**
  * @}
  */

/** @addtogroup LTDC_Exported_Functions_Group2
  * @{
  */
/* IO operation functions *****************************************************/
void _LTDC_IRQHandler(LTDC_HandleTypeDef *hltdc);
/**
  * @}
  */

/** @addtogroup LTDC_Exported_Functions_Group3
  * @{
  */
/* Peripheral Control functions ***********************************************/
void LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, unsigned int LayerIdx);
void LTDC_SetWindowSize(LTDC_HandleTypeDef *hltdc, unsigned int XSize, unsigned int YSize, unsigned int LayerIdx);
void LTDC_SetWindowPosition(LTDC_HandleTypeDef *hltdc, unsigned int X0, unsigned int Y0, unsigned int LayerIdx);
void LTDC_SetPixelFormat(LTDC_HandleTypeDef *hltdc, unsigned int Pixelformat, unsigned int LayerIdx);
void LTDC_SetAlpha(LTDC_HandleTypeDef *hltdc, unsigned int Alpha, unsigned int LayerIdx);
void LTDC_SetAddress(LTDC_HandleTypeDef *hltdc, unsigned int Address, unsigned int LayerIdx);
void LTDC_SetPitch(LTDC_HandleTypeDef *hltdc, unsigned int LinePitchInPixels, unsigned int LayerIdx);
void LTDC_ConfigColorKeying(LTDC_HandleTypeDef *hltdc, unsigned int RGBValue, unsigned int LayerIdx);
void LTDC_ConfigCLUT(LTDC_HandleTypeDef *hltdc, unsigned int *pCLUT, unsigned int CLUTSize, unsigned int LayerIdx);
void LTDC_EnableColorKeying(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);
void LTDC_DisableColorKeying(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);
void LTDC_EnableCLUT(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);
void LTDC_DisableCLUT(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);
void LTDC_ProgramLineEvent(LTDC_HandleTypeDef *hltdc, unsigned int Line);
void LTDC_EnableDither(LTDC_HandleTypeDef *hltdc);
void LTDC_DisableDither(LTDC_HandleTypeDef *hltdc);
void LTDC_Reload(LTDC_HandleTypeDef *hltdc, unsigned int ReloadType);
void LTDC_ConfigLayer_NoReload(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, unsigned int LayerIdx);
void LTDC_SetWindowSize_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int XSize, unsigned int YSize, unsigned int LayerIdx);
void LTDC_SetWindowPosition_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int X0, unsigned int Y0, unsigned int LayerIdx);
void LTDC_SetPixelFormat_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int Pixelformat, unsigned int LayerIdx);
void LTDC_SetAlpha_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int Alpha, unsigned int LayerIdx);
void LTDC_SetAddress_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int Address, unsigned int LayerIdx);
void LTDC_SetPitch_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LinePitchInPixels, unsigned int LayerIdx);
void LTDC_ConfigColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int RGBValue, unsigned int LayerIdx);
void LTDC_EnableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);
void LTDC_DisableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);
void LTDC_EnableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);
void LTDC_DisableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx);

/**
  * @}
  */

/** @addtogroup LTDC_Exported_Functions_Group4
  * @{
  */
/* Peripheral State functions *************************************************/
LTDC_StateTypeDef LTDC_GetState(LTDC_HandleTypeDef *hltdc);
unsigned int LTDC_GetError(LTDC_HandleTypeDef *hltdc);

/* Private macros ------------------------------------------------------------*/
/** @defgroup LTDC_Private_Macros LTDC Private Macros
  * @{
  */
#define LTDC_LAYER(__HANDLE__, __LAYER__)         ((LTDC_Layer_TypeDef *)((unsigned int)(((unsigned int)((__HANDLE__)->Instance)) + 0x84 + (0x80*(__LAYER__)))))

#endif /* _LTDC_H */
