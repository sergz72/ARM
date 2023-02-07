#ifndef _DMA2D_H
#define _DMA2D_H

/** 
  * @brief  DMA2D Init structure definition  
  */

typedef struct
{
  uint32_t DMA2D_Mode;                           /*!< configures the DMA2D transfer mode.
                                                 This parameter can be one value of @ref DMA2D_MODE */
  
  uint32_t DMA2D_CMode;                          /*!< configures the color format of the output image.
                                                 This parameter can be one value of @ref DMA2D_CMODE */

  uint32_t DMA2D_OutputBlue;                     /*!< configures the blue value of the output image. 
                                                 This parameter must range:
                                                 - from 0x00 to 0xFF if ARGB8888 color mode is slected
                                                 - from 0x00 to 0xFF if RGB888 color mode is slected
                                                 - from 0x00 to 0x1F if RGB565 color mode is slected
                                                 - from 0x00 to 0x1F if ARGB1555 color mode is slected
                                                 - from 0x00 to 0x0F if ARGB4444 color mode is slected  */

  uint32_t DMA2D_OutputGreen;                    /*!< configures the green value of the output image. 
                                                 This parameter must range:
                                                 - from 0x00 to 0xFF if ARGB8888 color mode is selected
                                                 - from 0x00 to 0xFF if RGB888 color mode is selected
                                                 - from 0x00 to 0x2F if RGB565 color mode is selected
                                                 - from 0x00 to 0x1F if ARGB1555 color mode is selected
                                                 - from 0x00 to 0x0F if ARGB4444 color mode is selected  */
            
  uint32_t DMA2D_OutputRed;                      /*!< configures the red value of the output image. 
                                                 This parameter must range:
                                                 - from 0x00 to 0xFF if ARGB8888 color mode is slected
                                                 - from 0x00 to 0xFF if RGB888 color mode is slected
                                                 - from 0x00 to 0x1F if RGB565 color mode is slected
                                                 - from 0x00 to 0x1F if ARGB1555 color mode is slected
                                                 - from 0x00 to 0x0F if ARGB4444 color mode is slected  */
  
  uint32_t DMA2D_OutputAlpha;                    /*!< configures the alpha channel of the output color. 
                                                 This parameter must range:
                                                 - from 0x00 to 0xFF if ARGB8888 color mode is selected
                                                 - from 0x00 to 0x01 if ARGB1555 color mode is selected
                                                 - from 0x00 to 0x0F if ARGB4444 color mode is selected  */

  uint32_t DMA2D_OutputMemoryAdd;                /*!< Specifies the memory address. This parameter 
                                                 must be range from 0x00000000 to 0xFFFFFFFF. */

  uint32_t DMA2D_OutputOffset;                   /*!< Specifies the Offset value. This parameter must be range from
                                                 0x0000 to 0x3FFF. */

  uint32_t DMA2D_NumberOfLine;                   /*!< Configures the number of line of the area to be transfered.
                                                 This parameter must range from 0x0000 to 0xFFFF */
            
  uint32_t DMA2D_PixelPerLine;                   /*!< Configures the number pixel per line of the area to be transferred.
                                                 This parameter must range from 0x0000 to 0x3FFF */
} DMA2D_InitTypeDef;



typedef struct
{
  uint32_t DMA2D_FGMA;                           /*!< configures the DMA2D foreground memory address.
                                                 This parameter must be range from 0x00000000 to 0xFFFFFFFF. */
  
  uint32_t DMA2D_FGO;                            /*!< configures the DMA2D foreground offset.
                                                 This parameter must be range from 0x0000 to 0x3FFF. */

  uint32_t DMA2D_FGCM;                           /*!< configures the DMA2D foreground color mode . 
                                                 This parameter can be one value of @ref DMA2D_FGCM */

  uint32_t DMA2D_FG_CLUT_CM;                     /*!< configures the DMA2D foreground CLUT color mode. 
                                                 This parameter can be one value of @ref DMA2D_FG_CLUT_CM */
            
  uint32_t DMA2D_FG_CLUT_SIZE;                   /*!< configures the DMA2D foreground CLUT size. 
                                                 This parameter must range from 0x00 to 0xFF. */
  
  uint32_t DMA2D_FGPFC_ALPHA_MODE;               /*!< configures the DMA2D foreground alpha mode. 
                                                 This parameter can be one value of @ref DMA2D_FGPFC_ALPHA_MODE */

  uint32_t DMA2D_FGPFC_ALPHA_VALUE;              /*!< Specifies the DMA2D foreground alpha value 
                                                 must be range from 0x00 to 0xFF. */

  uint32_t DMA2D_FGC_BLUE;                       /*!< Specifies the DMA2D foreground blue value 
                                                 must be range from 0x00 to 0xFF. */

  uint32_t DMA2D_FGC_GREEN;                      /*!< Specifies the DMA2D foreground green value 
                                                 must be range from 0x00 to 0xFF. */

  uint32_t DMA2D_FGC_RED;                        /*!< Specifies the DMA2D foreground red value 
                                                 must be range from 0x00 to 0xFF. */
            
  uint32_t DMA2D_FGCMAR;                         /*!< Configures the DMA2D foreground CLUT memory address.
                                                 This parameter must range from 0x00000000 to 0xFFFFFFFF. */
} DMA2D_FG_InitTypeDef;


typedef struct
{
  uint32_t DMA2D_BGMA;                           /*!< configures the DMA2D background memory address.
                                                 This parameter must be range from 0x00000000 to 0xFFFFFFFF. */
  
  uint32_t DMA2D_BGO;                            /*!< configures the DMA2D background offset.
                                                 This parameter must be range from 0x0000 to 0x3FFF. */

  uint32_t DMA2D_BGCM;                           /*!< configures the DMA2D background color mode . 
                                                 This parameter can be one value of @ref DMA2D_FGCM */

  uint32_t DMA2D_BG_CLUT_CM;                     /*!< configures the DMA2D background CLUT color mode. 
                                                 This parameter can be one value of @ref DMA2D_FG_CLUT_CM */
            
  uint32_t DMA2D_BG_CLUT_SIZE;                   /*!< configures the DMA2D background CLUT size. 
                                                 This parameter must range from 0x00 to 0xFF. */
  
  uint32_t DMA2D_BGPFC_ALPHA_MODE;               /*!< configures the DMA2D background alpha mode. 
                                                 This parameter can be one value of @ref DMA2D_FGPFC_ALPHA_MODE */

  uint32_t DMA2D_BGPFC_ALPHA_VALUE;              /*!< Specifies the DMA2D background alpha value 
                                                 must be range from 0x00 to 0xFF. */

  uint32_t DMA2D_BGC_BLUE;                       /*!< Specifies the DMA2D background blue value 
                                                 must be range from 0x00 to 0xFF. */

  uint32_t DMA2D_BGC_GREEN;                      /*!< Specifies the DMA2D background green value 
                                                 must be range from 0x00 to 0xFF. */

  uint32_t DMA2D_BGC_RED;                        /*!< Specifies the DMA2D background red value 
                                                 must be range from 0x00 to 0xFF. */
            
  uint32_t DMA2D_BGCMAR;                         /*!< Configures the DMA2D background CLUT memory address.
                                                 This parameter must range from 0x00000000 to 0xFFFFFFFF. */
} DMA2D_BG_InitTypeDef;



/* Exported constants --------------------------------------------------------*/

#define DMA2D_M2M                            ((uint32_t)0x00000000)
#define DMA2D_M2M_PFC                        ((uint32_t)0x00010000)
#define DMA2D_M2M_BLEND                      ((uint32_t)0x00020000)
#define DMA2D_R2M                            ((uint32_t)0x00030000)

#define DMA2D_ARGB8888                       ((uint32_t)0x00000000)
#define DMA2D_RGB888                         ((uint32_t)0x00000001)
#define DMA2D_RGB565                         ((uint32_t)0x00000002)
#define DMA2D_ARGB1555                       ((uint32_t)0x00000003)
#define DMA2D_ARGB4444                       ((uint32_t)0x00000004)

#define DMA2D_Output_Color                 ((uint32_t)0x000000FF)

#define DMA2D_OUTPUT_OFFSET      ((uint32_t)0x00003FFF)

#define DMA2D_pixel          ((uint32_t)0x00003FFF)
#define DMA2D_Line           ((uint32_t)0x0000FFFF)

#define CM_ARGB8888        ((uint32_t)0x00000000)
#define CM_RGB888          ((uint32_t)0x00000001)
#define CM_RGB565          ((uint32_t)0x00000002)
#define CM_ARGB1555        ((uint32_t)0x00000003)
#define CM_ARGB4444        ((uint32_t)0x00000004)
#define CM_L8              ((uint32_t)0x00000005)
#define CM_AL44            ((uint32_t)0x00000006)
#define CM_AL88            ((uint32_t)0x00000007)
#define CM_L4              ((uint32_t)0x00000008)
#define CM_A8              ((uint32_t)0x00000009)
#define CM_A4              ((uint32_t)0x0000000A)

#define CLUT_CM_ARGB8888        ((uint32_t)0x00000000)
#define CLUT_CM_RGB888          ((uint32_t)0x00000001)

#define NO_MODIF_ALPHA_VALUE       ((uint32_t)0x00000000)
#define REPLACE_ALPHA_VALUE        ((uint32_t)0x00000001)
#define COMBINE_ALPHA_VALUE        ((uint32_t)0x00000002)

#define DMA2D_IT_CE                      DMA2D_CR_CEIE
#define DMA2D_IT_CTC                     DMA2D_CR_CTCIE
#define DMA2D_IT_CAE                     DMA2D_CR_CAEIE
#define DMA2D_IT_TW                      DMA2D_CR_TWIE
#define DMA2D_IT_TC                      DMA2D_CR_TCIE
#define DMA2D_IT_TE                      DMA2D_CR_TEIE

#define DMA2D_FLAG_CE                      DMA2D_ISR_CEIF
#define DMA2D_FLAG_CTC                     DMA2D_ISR_CTCIF
#define DMA2D_FLAG_CAE                     DMA2D_ISR_CAEIF
#define DMA2D_FLAG_TW                      DMA2D_ISR_TWIF
#define DMA2D_FLAG_TC                      DMA2D_ISR_TCIF
#define DMA2D_FLAG_TE                      DMA2D_ISR_TEIF

/* Exported functions ------------------------------------------------------- */

/*  Function used to set the DMA2D configuration to the default reset state *****/
void DMA2D_DeInit(void);

/* Initialization and Configuration functions *********************************/
void DMA2D_Init(DMA2D_InitTypeDef* DMA2D_InitStruct);
void DMA2D_StructInit(DMA2D_InitTypeDef* DMA2D_InitStruct);
void DMA2D_StartTransfer(void);
void DMA2D_AbortTransfer(void);
void DMA2D_Suspend(FunctionalState NewState);
void DMA2D_FGConfig(DMA2D_FG_InitTypeDef* DMA2D_FG_InitStruct);
void DMA2D_FG_StructInit(DMA2D_FG_InitTypeDef* DMA2D_FG_InitStruct);
void DMA2D_BGConfig(DMA2D_BG_InitTypeDef* DMA2D_BG_InitStruct);
void DMA2D_BG_StructInit(DMA2D_BG_InitTypeDef* DMA2D_BG_InitStruct);
void DMA2D_FGStart(FunctionalState NewState);
void DMA2D_BGStart(FunctionalState NewState);
void DMA2D_DeadTimeConfig(uint32_t DMA2D_DeadTime, FunctionalState NewState);
void DMA2D_LineWatermarkConfig(uint32_t DMA2D_LWatermarkConfig);

/* Interrupts and flags management functions **********************************/
void DMA2D_ITConfig(uint32_t DMA2D_IT, FunctionalState NewState);
FlagStatus DMA2D_GetFlagStatus(uint32_t DMA2D_FLAG);
void DMA2D_ClearFlag(uint32_t DMA2D_FLAG);
ITStatus DMA2D_GetITStatus(uint32_t DMA2D_IT);
void DMA2D_ClearITPendingBit(uint32_t DMA2D_IT);

#endif /* _DMA2D_H */
