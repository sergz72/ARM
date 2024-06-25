#include "board.h"
#include "dma2d.h"

/** @defgroup DMA2D_TimeOut DMA2D Time Out 
  * @{
  */  
#define DMA2D_TIMEOUT_ABORT           ((unsigned int)1000000)  /*!<  1s  */
#define DMA2D_TIMEOUT_SUSPEND         ((unsigned int)1000000)  /*!<  1s  */
/**
  * @}
  */

/** @defgroup DMA2D_Shifts DMA2D Shifts 
  * @{
  */                                     
#define DMA2D_POSITION_FGPFCCR_CS     DMA2D_FGPFCCR_CS_Pos    /*!< Required left shift to set foreground CLUT size */
#define DMA2D_POSITION_BGPFCCR_CS     DMA2D_BGPFCCR_CS_Pos    /*!< Required left shift to set background CLUT size */
                                                                                 
#define DMA2D_POSITION_FGPFCCR_CCM    DMA2D_FGPFCCR_CCM_Pos   /*!< Required left shift to set foreground CLUT color mode */
#define DMA2D_POSITION_BGPFCCR_CCM    DMA2D_BGPFCCR_CCM_Pos   /*!< Required left shift to set background CLUT color mode */
                                                                                 
#define DMA2D_POSITION_OPFCCR_AI      DMA2D_OPFCCR_AI_Pos     /*!< Required left shift to set output alpha inversion     */
#define DMA2D_POSITION_FGPFCCR_AI     DMA2D_FGPFCCR_AI_Pos    /*!< Required left shift to set foreground alpha inversion */
#define DMA2D_POSITION_BGPFCCR_AI     DMA2D_BGPFCCR_AI_Pos    /*!< Required left shift to set background alpha inversion */

#define DMA2D_POSITION_OPFCCR_RBS     DMA2D_OPFCCR_RBS_Pos    /*!< Required left shift to set output Red/Blue swap     */
#define DMA2D_POSITION_FGPFCCR_RBS    DMA2D_FGPFCCR_RBS_Pos   /*!< Required left shift to set foreground Red/Blue swap */
#define DMA2D_POSITION_BGPFCCR_RBS    DMA2D_BGPFCCR_RBS_Pos   /*!< Required left shift to set background Red/Blue swap */
                                                                                 
#define DMA2D_POSITION_AMTCR_DT       DMA2D_AMTCR_DT_Pos      /*!< Required left shift to set deadtime value */
                                                                                 
#define DMA2D_POSITION_FGPFCCR_AM     DMA2D_FGPFCCR_AM_Pos    /*!< Required left shift to set foreground alpha mode */
#define DMA2D_POSITION_BGPFCCR_AM     DMA2D_BGPFCCR_AM_Pos    /*!< Required left shift to set background alpha mode */

#define DMA2D_POSITION_FGPFCCR_ALPHA  DMA2D_FGPFCCR_ALPHA_Pos /*!< Required left shift to set foreground alpha value */
#define DMA2D_POSITION_BGPFCCR_ALPHA  DMA2D_BGPFCCR_ALPHA_Pos /*!< Required left shift to set background alpha value */

#define DMA2D_POSITION_NLR_PL         DMA2D_NLR_PL_Pos        /*!< Required left shift to set pixels per lines value */
/**
  * @}
  */

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup DMA2D_Private_Functions_Prototypes
  * @{
  */
static void DMA2D_SetConfig(DMA2D_HandleTypeDef *hdma2d, unsigned int pdata, unsigned int DstAddress, unsigned int Width, unsigned int Height);

/**
  * @brief  Initialize the DMA2D according to the specified
  *         parameters in the DMA2D_InitTypeDef and create the associated handle.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval HAL status
  */
unsigned int DMA2D_Init(DMA2D_HandleTypeDef *hdma2d)
{ 
  /* Check the DMA2D peripheral state */
  if(hdma2d == NULL)
  {
     return 1;
  }

  if(hdma2d->State == DMA2D_STATE_RESET)
  {
    /* Init the low level hardware */
    DMA2D_MspInit(hdma2d);
  }
  
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;

  /* DMA2D CR register configuration -------------------------------------------*/
  MODIFY_REG(hdma2d->Instance->CR, DMA2D_CR_MODE, hdma2d->Init.Mode);

  /* DMA2D OPFCCR register configuration ---------------------------------------*/
  MODIFY_REG(hdma2d->Instance->OPFCCR, DMA2D_OPFCCR_CM, hdma2d->Init.ColorMode);

  /* DMA2D OOR register configuration ------------------------------------------*/  
  MODIFY_REG(hdma2d->Instance->OOR, DMA2D_OOR_LO, hdma2d->Init.OutputOffset);  

#if defined (DMA2D_OPFCCR_AI)
  /* DMA2D OPFCCR AI fields setting (Output Alpha Inversion)*/
  MODIFY_REG(hdma2d->Instance->OPFCCR, DMA2D_OPFCCR_AI, (hdma2d->Init.AlphaInverted << DMA2D_POSITION_OPFCCR_AI));
#endif /* DMA2D_OPFCCR_AI */ 
  
#if defined (DMA2D_OPFCCR_RBS) 
  MODIFY_REG(hdma2d->Instance->OPFCCR, DMA2D_OPFCCR_RBS,(hdma2d->Init.RedBlueSwap << DMA2D_POSITION_OPFCCR_RBS));
#endif /* DMA2D_OPFCCR_RBS */
  

  /* Update error code */
  hdma2d->ErrorCode = DMA2D_ERROR_NONE;

  /* Initialize the DMA2D state*/
  hdma2d->State  = DMA2D_STATE_READY;

  return 0;
}

/**
  * @brief  Deinitializes the DMA2D peripheral registers to their default reset
  *         values.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */

unsigned int DMA2D_DeInit(DMA2D_HandleTypeDef *hdma2d)
{
  
  /* Check the DMA2D peripheral state */
  if(hdma2d == NULL)
  {
     return 1;
  }
  
  /* Before aborting any DMA2D transfer or CLUT loading, check
     first whether or not DMA2D clock is enabled */
  if (RCC->AHB1ENR & RCC_AHB1ENR_DMA2DEN)
  {
    /* Abort DMA2D transfer if any */
    if ((hdma2d->Instance->CR & DMA2D_CR_START) == DMA2D_CR_START)
    {
      if (DMA2D_Abort(hdma2d))
      {
        /* Issue when aborting DMA2D transfer */       
        return 1;
      }
    }
    else
    {
      /* Abort background CLUT loading if any */
      if ((hdma2d->Instance->BGPFCCR & DMA2D_BGPFCCR_START) == DMA2D_BGPFCCR_START)
      {  
        if (DMA2D_CLUTLoading_Abort(hdma2d, 0))
        {
          /* Issue when aborting background CLUT loading */     
          return 1;
        }
      }
      else
      {
        /* Abort foreground CLUT loading if any */
        if ((hdma2d->Instance->FGPFCCR & DMA2D_FGPFCCR_START) == DMA2D_FGPFCCR_START)
        {
          if (DMA2D_CLUTLoading_Abort(hdma2d, 1))
          {
            /* Issue when aborting foreground CLUT loading */     
            return 1;
          }        
        }
      }
    }
  }

            
  /* Carry on with de-initialization of low level hardware */
  DMA2D_MspDeInit(hdma2d);
  
  /* Reset DMA2D control registers*/
  hdma2d->Instance->CR = 0;
  hdma2d->Instance->FGOR = 0;
  hdma2d->Instance->BGOR = 0;  
  hdma2d->Instance->FGPFCCR = 0;
  hdma2d->Instance->BGPFCCR = 0;  
  hdma2d->Instance->OPFCCR = 0;

  /* Update error code */
  hdma2d->ErrorCode = DMA2D_ERROR_NONE;
  
  /* Initialize the DMA2D state*/
  hdma2d->State  = DMA2D_STATE_RESET;

  return 0;
}

/**
  * @brief  Initializes the DMA2D MSP.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */
__weak void DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
}

/**
  * @brief  DeInitializes the DMA2D MSP.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */
__weak void DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
}

/**
  * @brief  Start the DMA2D Transfer.
  * @param  hdma2d:     Pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the DMA2D.  
  * @param  pdata:      Configure the source memory Buffer address if 
  *                     Memory-to-Memory or Memory-to-Memory with pixel format 
  *                     conversion mode is selected, or configure 
  *                     the color value if Register-to-Memory mode is selected.
  * @param  DstAddress: The destination memory Buffer address.
  * @param  Width:      The width of data to be transferred from source to destination (expressed in number of pixels per line).
  * @param  Height:     The height of data to be transferred from source to destination (expressed in number of lines).
  * @retval HAL status
  */
void DMA2D_Start(DMA2D_HandleTypeDef *hdma2d, unsigned int pdata, unsigned int DstAddress, unsigned int Width,  unsigned int Height)
{
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;
  
  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, pdata, DstAddress, Width, Height);

  /* Enable the Peripheral */
  __DMA2D_ENABLE(hdma2d);
}

/**
  * @brief  Start the DMA2D Transfer with interrupt enabled.
  * @param  hdma2d:     Pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the DMA2D.  
  * @param  pdata:      Configure the source memory Buffer address if 
  *                     the Memory-to-Memory or Memory-to-Memory with pixel format 
  *                     conversion mode is selected, or configure 
  *                     the color value if Register-to-Memory mode is selected.
  * @param  DstAddress: The destination memory Buffer address.
  * @param  Width:      The width of data to be transferred from source to destination (expressed in number of pixels per line).
  * @param  Height:     The height of data to be transferred from source to destination (expressed in number of lines).
  * @retval HAL status
  */
void DMA2D_Start_IT(DMA2D_HandleTypeDef *hdma2d, unsigned int pdata, unsigned int DstAddress, unsigned int Width,  unsigned int Height)
{
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, pdata, DstAddress, Width, Height);

  /* Enable the transfer complete, transfer error and configuration error interrupts */
  __DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE);

  /* Enable the Peripheral */
  __DMA2D_ENABLE(hdma2d);
}

/**
  * @brief  Start the multi-source DMA2D Transfer.
  * @param  hdma2d:      Pointer to a DMA2D_HandleTypeDef structure that contains
  *                      the configuration information for the DMA2D.  
  * @param  SrcAddress1: The source memory Buffer address for the foreground layer.
  * @param  SrcAddress2: The source memory Buffer address for the background layer.
  * @param  DstAddress:  The destination memory Buffer address.
  * @param  Width:       The width of data to be transferred from source to destination (expressed in number of pixels per line).
  * @param  Height:      The height of data to be transferred from source to destination (expressed in number of lines).
  * @retval HAL status
  */
void DMA2D_BlendingStart(DMA2D_HandleTypeDef *hdma2d, unsigned int SrcAddress1, unsigned int  SrcAddress2, unsigned int DstAddress, unsigned int Width,  unsigned int Height)
{
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;

  /* Configure DMA2D Stream source2 address */
  WRITE_REG(hdma2d->Instance->BGMAR, SrcAddress2);

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, SrcAddress1, DstAddress, Width, Height);

  /* Enable the Peripheral */
  __DMA2D_ENABLE(hdma2d);
}

/**
  * @brief  Start the multi-source DMA2D Transfer with interrupt enabled.
  * @param  hdma2d:     Pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the DMA2D.  
  * @param  SrcAddress1: The source memory Buffer address for the foreground layer.
  * @param  SrcAddress2: The source memory Buffer address for the background layer.
  * @param  DstAddress:  The destination memory Buffer address.
  * @param  Width:       The width of data to be transferred from source to destination (expressed in number of pixels per line).
  * @param  Height:      The height of data to be transferred from source to destination (expressed in number of lines).         
  * @retval HAL status
  */
void DMA2D_BlendingStart_IT(DMA2D_HandleTypeDef *hdma2d, unsigned int SrcAddress1, unsigned int  SrcAddress2, unsigned int DstAddress, unsigned int Width,  unsigned int Height)
{
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;
 
  /* Configure DMA2D Stream source2 address */
  WRITE_REG(hdma2d->Instance->BGMAR, SrcAddress2);

  /* Configure the source, destination address and the data size */
  DMA2D_SetConfig(hdma2d, SrcAddress1, DstAddress, Width, Height);
  
  /* Enable the transfer complete, transfer error and configuration error interrupts */
  __DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE);
  
  /* Enable the Peripheral */
  __DMA2D_ENABLE(hdma2d);
}

/**
  * @brief  Abort the DMA2D Transfer.
  * @param  hdma2d : pointer to a DMA2D_HandleTypeDef structure that contains
  *                  the configuration information for the DMA2D.
  * @retval HAL status
  */
unsigned int DMA2D_Abort(DMA2D_HandleTypeDef *hdma2d)
{
  unsigned int tickstart;

  /* Abort the DMA2D transfer */
  /* START bit is reset to make sure not to set it again, in the event the HW clears it
     between the register read and the register write by the CPU (writing ‘0’ has no 
     effect on START bitvalue). */
  MODIFY_REG(hdma2d->Instance->CR, DMA2D_CR_ABORT|DMA2D_CR_START, DMA2D_CR_ABORT);

  /* Get tick */
  tickstart = 0;

  /* Check if the DMA2D is effectively disabled */
  while((hdma2d->Instance->CR & DMA2D_CR_START) != RESET)
  {
    if(tickstart++ > DMA2D_TIMEOUT_ABORT)
    {
      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_TIMEOUT;
      
      /* Change the DMA2D state */
      hdma2d->State = DMA2D_STATE_TIMEOUT;
      
      return 1;
    }
  }

  /* Disable the Transfer Complete, Transfer Error and Configuration Error interrupts */
  __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TC|DMA2D_IT_TE|DMA2D_IT_CE);

  /* Change the DMA2D state*/
  hdma2d->State = DMA2D_STATE_READY;

  return 0;
}

/**
  * @brief  Suspend the DMA2D Transfer.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D. 
  * @retval HAL status
  */
unsigned int DMA2D_Suspend(DMA2D_HandleTypeDef *hdma2d)
{
  unsigned int tickstart;

  /* Suspend the DMA2D transfer */
  /* START bit is reset to make sure not to set it again, in the event the HW clears it
     between the register read and the register write by the CPU (writing ‘0’ has no 
     effect on START bitvalue). */
  MODIFY_REG(hdma2d->Instance->CR, DMA2D_CR_SUSP|DMA2D_CR_START, DMA2D_CR_SUSP);

  /* Get tick */
  tickstart = 0;

  /* Check if the DMA2D is effectively suspended */
  while (((hdma2d->Instance->CR & DMA2D_CR_SUSP) != DMA2D_CR_SUSP) \
    && ((hdma2d->Instance->CR & DMA2D_CR_START) == DMA2D_CR_START))
  {
    if(tickstart++ > DMA2D_TIMEOUT_SUSPEND)
    {
      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_TIMEOUT;
      
      /* Change the DMA2D state */
      hdma2d->State = DMA2D_STATE_TIMEOUT;
      
      return 1;
    }
  }
  
   /* Check whether or not a transfer is actually suspended and change the DMA2D state accordingly */
  if ((hdma2d->Instance->CR & DMA2D_CR_START) != RESET)
  {    
    hdma2d->State = DMA2D_STATE_SUSPEND;
  }
  else
  {
    /* Make sure SUSP bit is cleared since it is meaningless 
       when no tranfer is on-going */
    CLEAR_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP);
  }

  return 0;
}

/**
  * @brief  Resume the DMA2D Transfer.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.  
  * @retval HAL status
  */
void DMA2D_Resume(DMA2D_HandleTypeDef *hdma2d)
{
  /* Check the SUSP and START bits */
  if((hdma2d->Instance->CR & (DMA2D_CR_SUSP | DMA2D_CR_START)) == (DMA2D_CR_SUSP | DMA2D_CR_START))
  {
    /* Ongoing transfer is suspended: change the DMA2D state before resuming */
    hdma2d->State = DMA2D_STATE_BUSY;
  }

  /* Resume the DMA2D transfer */
  /* START bit is reset to make sure not to set it again, in the event the HW clears it
     between the register read and the register write by the CPU (writing ‘0’ has no 
     effect on START bitvalue). */
  CLEAR_BIT(hdma2d->Instance->CR, (DMA2D_CR_SUSP|DMA2D_CR_START));  
}


/**
  * @brief  Enable the DMA2D CLUT Transfer.
  * @param  hdma2d:   Pointer to a DMA2D_HandleTypeDef structure that contains
  *                   the configuration information for the DMA2D.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
void DMA2D_EnableCLUT(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx)
{  
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;
  
  if(LayerIdx == 0)
  {
    /* Enable the background CLUT loading */
    SET_BIT(hdma2d->Instance->BGPFCCR, DMA2D_BGPFCCR_START);
  }
  else
  {
    /* Enable the foreground CLUT loading */
    SET_BIT(hdma2d->Instance->FGPFCCR, DMA2D_FGPFCCR_START);    
  }
}


/**
  * @brief  Start DMA2D CLUT Loading.
  * @param  hdma2d:   Pointer to a DMA2D_HandleTypeDef structure that contains
  *                   the configuration information for the DMA2D.
  * @param  CLUTCfg:  Pointer to a DMA2D_CLUTCfgTypeDef structure that contains
  *                   the configuration information for the color look up table.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @note Invoking this API is similar to calling HAL_DMA2D_ConfigCLUT() then HAL_DMA2D_EnableCLUT().                    
  * @retval HAL status
  */
void DMA2D_CLUTLoad(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, unsigned int LayerIdx)
{
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;
    
  /* Configure the CLUT of the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write background CLUT memory address */
    WRITE_REG(hdma2d->Instance->BGCMAR, (unsigned int)CLUTCfg.pCLUT);
    
    /* Write background CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, (DMA2D_BGPFCCR_CS | DMA2D_BGPFCCR_CCM), 
            ((CLUTCfg.Size << DMA2D_POSITION_BGPFCCR_CS) | (CLUTCfg.CLUTColorMode << DMA2D_POSITION_BGPFCCR_CCM)));

    /* Enable the CLUT loading for the background */
    SET_BIT(hdma2d->Instance->BGPFCCR, DMA2D_BGPFCCR_START);
  }
  /* Configure the CLUT of the foreground DMA2D layer */
  else
  {
    /* Write foreground CLUT memory address */
    WRITE_REG(hdma2d->Instance->FGCMAR, (unsigned int)CLUTCfg.pCLUT);
    
    /* Write foreground CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM), 
            ((CLUTCfg.Size << DMA2D_POSITION_BGPFCCR_CS) | (CLUTCfg.CLUTColorMode << DMA2D_POSITION_FGPFCCR_CCM)));       
    
 /* Enable the CLUT loading for the foreground */
    SET_BIT(hdma2d->Instance->FGPFCCR, DMA2D_FGPFCCR_START);  
  }
}

/**
  * @brief  Start DMA2D CLUT Loading with interrupt enabled.
  * @param  hdma2d:   Pointer to a DMA2D_HandleTypeDef structure that contains
  *                   the configuration information for the DMA2D.
  * @param  CLUTCfg:  Pointer to a DMA2D_CLUTCfgTypeDef structure that contains
  *                   the configuration information for the color look up table.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
void DMA2D_CLUTLoad_IT(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, unsigned int LayerIdx)
{
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;
    
  /* Configure the CLUT of the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write background CLUT memory address */
    WRITE_REG(hdma2d->Instance->BGCMAR, (unsigned int)CLUTCfg.pCLUT);
    
    /* Write background CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, (DMA2D_BGPFCCR_CS | DMA2D_BGPFCCR_CCM), 
            ((CLUTCfg.Size << DMA2D_POSITION_BGPFCCR_CS) | (CLUTCfg.CLUTColorMode << DMA2D_POSITION_BGPFCCR_CCM)));
            
    /* Enable the CLUT Transfer Complete, transfer Error, configuration Error and CLUT Access Error interrupts */
    __DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_CTC | DMA2D_IT_TE | DMA2D_IT_CE |DMA2D_IT_CAE);

    /* Enable the CLUT loading for the background */
    SET_BIT(hdma2d->Instance->BGPFCCR, DMA2D_BGPFCCR_START);
  }
  /* Configure the CLUT of the foreground DMA2D layer */
  else
  {
    /* Write foreground CLUT memory address */
    WRITE_REG(hdma2d->Instance->FGCMAR, (unsigned int)CLUTCfg.pCLUT);
    
    /* Write foreground CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM), 
            ((CLUTCfg.Size << DMA2D_POSITION_BGPFCCR_CS) | (CLUTCfg.CLUTColorMode << DMA2D_POSITION_FGPFCCR_CCM)));
            
    /* Enable the CLUT Transfer Complete, transfer Error, configuration Error and CLUT Access Error interrupts */
    __DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_CTC | DMA2D_IT_TE | DMA2D_IT_CE |DMA2D_IT_CAE);
    
    /* Enable the CLUT loading for the foreground */
    SET_BIT(hdma2d->Instance->FGPFCCR, DMA2D_FGPFCCR_START);  
  }
}

/**
  * @brief  Abort the DMA2D CLUT loading.
  * @param  hdma2d : Pointer to a DMA2D_HandleTypeDef structure that contains
  *                  the configuration information for the DMA2D.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)  
  * @retval HAL status
  */
unsigned int DMA2D_CLUTLoading_Abort(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx)
{
  unsigned int tickstart;
  __IO uint32_t * reg =  &(hdma2d->Instance->BGPFCCR); /* by default, point at background register */

  /* Abort the CLUT loading */
  SET_BIT(hdma2d->Instance->CR, DMA2D_CR_ABORT);
  
  /* If foreground CLUT loading is considered, update local variables */ 
  if(LayerIdx == 1)
  {
    reg  = &(hdma2d->Instance->FGPFCCR);
  }


  /* Get tick */
  tickstart = 0;
 
  /* Check if the CLUT loading is aborted */          
  while((*reg & DMA2D_BGPFCCR_START) != RESET)
  {
    if(tickstart++ > DMA2D_TIMEOUT_ABORT)
    {
      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_TIMEOUT;
      
      /* Change the DMA2D state */
      hdma2d->State = DMA2D_STATE_TIMEOUT;
      
      return 1;
    }
  }

  /* Disable the CLUT Transfer Complete, Transfer Error, Configuration Error and CLUT Access Error interrupts */
  __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CTC | DMA2D_IT_TE | DMA2D_IT_CE |DMA2D_IT_CAE);
   
  /* Change the DMA2D state*/
  hdma2d->State = DMA2D_STATE_READY;

  return 0;
}

/**
  * @brief  Suspend the DMA2D CLUT loading.
  * @param  hdma2d: Pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D. 
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)    
  * @retval HAL status
  */
unsigned int DMA2D_CLUTLoading_Suspend(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx)
{
  unsigned int tickstart = 0;
  __IO uint32_t * reg =  &(hdma2d->Instance->BGPFCCR); /* by default, point at background register */

  /* Suspend the CLUT loading */
  SET_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP); 
  
  /* If foreground CLUT loading is considered, update local variables */ 
  if(LayerIdx == 1)
  {
    reg  = &(hdma2d->Instance->FGPFCCR);
  }   

  /* Get tick */
  tickstart = 0;
  
  /* Check if the CLUT loading is suspended */
  while (((hdma2d->Instance->CR & DMA2D_CR_SUSP) != DMA2D_CR_SUSP) \
    && ((*reg & DMA2D_BGPFCCR_START) == DMA2D_BGPFCCR_START))
  {
    if(tickstart++ > DMA2D_TIMEOUT_SUSPEND)
    {
      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_TIMEOUT;
      
      /* Change the DMA2D state */
      hdma2d->State = DMA2D_STATE_TIMEOUT;
      
      return 1;
    }
  }
  
   /* Check whether or not a transfer is actually suspended and change the DMA2D state accordingly */
  if ((*reg & DMA2D_BGPFCCR_START) != RESET)
  {    
    hdma2d->State = DMA2D_STATE_SUSPEND;
  }
  else
  {
    /* Make sure SUSP bit is cleared since it is meaningless 
       when no tranfer is on-going */
    CLEAR_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP);
  }  

  return 0;
}

/**
  * @brief  Resume the DMA2D CLUT loading.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D. 
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)      
  * @retval HAL status
  */
void DMA2D_CLUTLoading_Resume(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx)
{
  /* Check the SUSP and START bits for background or foreground CLUT loading */
  if(LayerIdx == 0)
  {  
    /* Background CLUT loading suspension check */
    if (((hdma2d->Instance->CR & DMA2D_CR_SUSP) == DMA2D_CR_SUSP)
      && ((hdma2d->Instance->BGPFCCR & DMA2D_BGPFCCR_START) == DMA2D_BGPFCCR_START))
    {
      /* Ongoing CLUT loading is suspended: change the DMA2D state before resuming */
      hdma2d->State = DMA2D_STATE_BUSY;
    }
  }
  else
  {
    /* Foreground CLUT loading suspension check */
    if (((hdma2d->Instance->CR & DMA2D_CR_SUSP) == DMA2D_CR_SUSP)
      && ((hdma2d->Instance->FGPFCCR & DMA2D_FGPFCCR_START) == DMA2D_FGPFCCR_START))
    {
      /* Ongoing CLUT loading is suspended: change the DMA2D state before resuming */
      hdma2d->State = DMA2D_STATE_BUSY;
    }  
  }

  /* Resume the CLUT loading */
  CLEAR_BIT(hdma2d->Instance->CR, DMA2D_CR_SUSP);  
}


/**

  * @brief  Polling for transfer complete or CLUT loading.
  * @param  hdma2d: Pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D. 
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
unsigned int DMA2D_PollForTransfer(DMA2D_HandleTypeDef *hdma2d, unsigned int Timeout)
{
  unsigned int tickstart;
  __IO unsigned int isrflags;

  /* Polling for DMA2D transfer */
  if((hdma2d->Instance->CR & DMA2D_CR_START) != RESET)
  {
   /* Get tick */
   tickstart = 0;

    while(__DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_TC) == RESET)
    {
      isrflags = READ_REG(hdma2d->Instance->ISR); 
      if ((isrflags & (DMA2D_FLAG_CE|DMA2D_FLAG_TE)) != RESET)
      {
        if ((isrflags & DMA2D_FLAG_CE) != RESET)
        {
          hdma2d->ErrorCode |= DMA2D_ERROR_CE;
        }
        if ((isrflags & DMA2D_FLAG_TE) != RESET)        
        {
          hdma2d->ErrorCode |= DMA2D_ERROR_TE;
        }
        /* Clear the transfer and configuration error flags */
        __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CE | DMA2D_FLAG_TE);

        /* Change DMA2D state */
        hdma2d->State = DMA2D_STATE_ERROR;

        return 1;
      }
      /* Check for the Timeout */
      if(Timeout != _MAX_DELAY)
      {
        if((Timeout == 0)||(tickstart++ > Timeout))
        {
          /* Update error code */
          hdma2d->ErrorCode |= DMA2D_ERROR_TIMEOUT;

          /* Change the DMA2D state */
          hdma2d->State = DMA2D_STATE_TIMEOUT;

          return 1;
        }
      }        
    }
  }
  /* Polling for CLUT loading (foreground or background) */
  if (((hdma2d->Instance->FGPFCCR & DMA2D_FGPFCCR_START) != RESET)  || 
      ((hdma2d->Instance->BGPFCCR & DMA2D_BGPFCCR_START) != RESET))
  {
    /* Get tick */
    tickstart = 0;
   
    while(__DMA2D_GET_FLAG(hdma2d, DMA2D_FLAG_CTC) == RESET)
    {
      isrflags = READ_REG(hdma2d->Instance->ISR);   
      if ((isrflags & (DMA2D_FLAG_CAE|DMA2D_FLAG_CE|DMA2D_FLAG_TE)) != RESET)        
      {      
        if ((isrflags & DMA2D_FLAG_CAE) != RESET)
        {
          hdma2d->ErrorCode |= DMA2D_ERROR_CAE;
        }   
        if ((isrflags & DMA2D_FLAG_CE) != RESET)             
        {
          hdma2d->ErrorCode |= DMA2D_ERROR_CE;
        }
        if ((isrflags & DMA2D_FLAG_TE) != RESET)        
        {
          hdma2d->ErrorCode |= DMA2D_ERROR_TE;
        }
        /* Clear the CLUT Access Error, Configuration Error and Transfer Error flags */
        __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CAE | DMA2D_FLAG_CE | DMA2D_FLAG_TE);
        
        /* Change DMA2D state */
        hdma2d->State= DMA2D_STATE_ERROR;
        
        return 1;
      }      
      /* Check for the Timeout */
      if(Timeout != _MAX_DELAY)
      {
        if((Timeout == 0)||(tickstart++ > Timeout))
        {
          /* Update error code */
          hdma2d->ErrorCode |= DMA2D_ERROR_TIMEOUT;
    
          /* Change the DMA2D state */
          hdma2d->State= DMA2D_STATE_TIMEOUT;
        
          return 1;
        }
      }      
    }
  }

  /* Clear the transfer complete and CLUT loading flags */
  __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TC|DMA2D_FLAG_CTC);
  
  /* Change DMA2D state */
  hdma2d->State = DMA2D_STATE_READY;
  
  return 0;
}

/**
  * @brief  Handle DMA2D interrupt request.
  * @param  hdma2d: Pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.  
  * @retval HAL status
  */
void _DMA2D_IRQHandler(DMA2D_HandleTypeDef *hdma2d)
{
  unsigned int isrflags = READ_REG(hdma2d->Instance->ISR);
  unsigned int crflags = READ_REG(hdma2d->Instance->CR);
        
  /* Transfer Error Interrupt management ***************************************/
  if ((isrflags & DMA2D_FLAG_TE) != RESET)
  {
    if ((crflags & DMA2D_IT_TE) != RESET)    
    {
      /* Disable the transfer Error interrupt */
      __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TE);

      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_TE;
    
      /* Clear the transfer error flag */
      __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TE);

      /* Change DMA2D state */
      hdma2d->State = DMA2D_STATE_ERROR;

      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }
  /* Configuration Error Interrupt management **********************************/
  if ((isrflags & DMA2D_FLAG_CE) != RESET)
  {
    if ((crflags & DMA2D_IT_CE) != RESET)    
    {  
      /* Disable the Configuration Error interrupt */
      __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CE);
  
      /* Clear the Configuration error flag */
      __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CE);

      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_CE;
    
      /* Change DMA2D state */
      hdma2d->State = DMA2D_STATE_ERROR;

      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }
  /* CLUT access Error Interrupt management ***********************************/
  if ((isrflags & DMA2D_FLAG_CAE) != RESET)
  {
    if ((crflags & DMA2D_IT_CAE) != RESET)    
    {    
      /* Disable the CLUT access error interrupt */
      __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CAE);
  
      /* Clear the CLUT access error flag */
      __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CAE);

      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_CAE;
    
      /* Change DMA2D state */
      hdma2d->State = DMA2D_STATE_ERROR;

      if(hdma2d->XferErrorCallback != NULL)
      {
        /* Transfer error Callback */
        hdma2d->XferErrorCallback(hdma2d);
      }
    }
  }  
  /* Transfer watermark Interrupt management **********************************/
  if ((isrflags & DMA2D_FLAG_TW) != RESET)
  {
    if ((crflags & DMA2D_IT_TW) != RESET)    
    {    
      /* Disable the transfer watermark interrupt */
      __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TW);
  
      /* Clear the transfer watermark flag */  
      __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TW);

      /* Transfer watermark Callback */
      DMA2D_LineEventCallback(hdma2d);
    }
  }  
  /* Transfer Complete Interrupt management ************************************/
  if ((isrflags & DMA2D_FLAG_TC) != RESET)
  {
    if ((crflags & DMA2D_IT_TC) != RESET)    
    {   
      /* Disable the transfer complete interrupt */
      __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_TC);
  
      /* Clear the transfer complete flag */  
      __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_TC);

      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_NONE;
    
      /* Change DMA2D state */
      hdma2d->State = DMA2D_STATE_READY;
    
      if(hdma2d->XferCpltCallback != NULL)
      {
        /* Transfer complete Callback */
        hdma2d->XferCpltCallback(hdma2d);
      }         
    }
  }
  /* CLUT Transfer Complete Interrupt management ******************************/
  if ((isrflags & DMA2D_FLAG_CTC) != RESET)
  {
    if ((crflags & DMA2D_IT_CTC) != RESET)    
    {    
      /* Disable the CLUT transfer complete interrupt */
      __DMA2D_DISABLE_IT(hdma2d, DMA2D_IT_CTC);
  
      /* Clear the CLUT transfer complete flag */  
      __DMA2D_CLEAR_FLAG(hdma2d, DMA2D_FLAG_CTC);

      /* Update error code */
      hdma2d->ErrorCode |= DMA2D_ERROR_NONE;
    
      /* Change DMA2D state */
      hdma2d->State = DMA2D_STATE_READY;
    
      /* CLUT Transfer complete Callback */
      DMA2D_CLUTLoadingCpltCallback(hdma2d);
    }
  }  
  
}

/**
  * @brief  Transfer watermark callback.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */
__weak void DMA2D_LineEventCallback(DMA2D_HandleTypeDef *hdma2d)
{
}

/**
  * @brief  CLUT Transfer Complete callback.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @retval None
  */
__weak void DMA2D_CLUTLoadingCpltCallback(DMA2D_HandleTypeDef *hdma2d)
{
}

/**
  * @brief  Configure the DMA2D Layer according to the specified
  *         parameters in the DMA2D_InitTypeDef and create the associated handle.
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
void DMA2D_ConfigLayer(DMA2D_HandleTypeDef *hdma2d, unsigned int LayerIdx)
{ 
  DMA2D_LayerCfgTypeDef *pLayerCfg = &hdma2d->LayerCfg[LayerIdx];
  
  unsigned int regMask = 0, regValue = 0;

  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;

  /* DMA2D BGPFCR register configuration -----------------------------------*/
  /* Prepare the value to be written to the BGPFCCR register */
  
  regValue = pLayerCfg->InputColorMode | (pLayerCfg->AlphaMode << DMA2D_POSITION_BGPFCCR_AM);
  regMask  = DMA2D_BGPFCCR_CM | DMA2D_BGPFCCR_AM | DMA2D_BGPFCCR_ALPHA;
  
#if defined (DMA2D_FGPFCCR_AI) && defined (DMA2D_BGPFCCR_AI)
  regValue |= (pLayerCfg->AlphaInverted << DMA2D_POSITION_BGPFCCR_AI);
  regMask  |= DMA2D_BGPFCCR_AI;  
#endif /* (DMA2D_FGPFCCR_AI) && (DMA2D_BGPFCCR_AI)  */ 
  
#if defined (DMA2D_FGPFCCR_RBS) && defined (DMA2D_BGPFCCR_RBS)
  regValue |= (pLayerCfg->RedBlueSwap << DMA2D_POSITION_BGPFCCR_RBS);
  regMask  |= DMA2D_BGPFCCR_RBS;  
#endif  
  
  if ((pLayerCfg->InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg->InputColorMode == DMA2D_INPUT_A8))
  {
    regValue |= (pLayerCfg->InputAlpha & DMA2D_BGPFCCR_ALPHA);
  }
  else
  {
    regValue |=  (pLayerCfg->InputAlpha << DMA2D_POSITION_BGPFCCR_ALPHA);
  }
  
  /* Configure the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write DMA2D BGPFCCR register */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, regMask, regValue);
              
    /* DMA2D BGOR register configuration -------------------------------------*/  
    WRITE_REG(hdma2d->Instance->BGOR, pLayerCfg->InputOffset);
    
    /* DMA2D BGCOLR register configuration -------------------------------------*/ 
    if ((pLayerCfg->InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg->InputColorMode == DMA2D_INPUT_A8))
    {    
      WRITE_REG(hdma2d->Instance->BGCOLR, pLayerCfg->InputAlpha & (DMA2D_BGCOLR_BLUE|DMA2D_BGCOLR_GREEN|DMA2D_BGCOLR_RED));
    }    
  }
  /* Configure the foreground DMA2D layer */
  else
  {
     /* Write DMA2D FGPFCCR register */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, regMask, regValue);
    
    /* DMA2D FGOR register configuration -------------------------------------*/
    WRITE_REG(hdma2d->Instance->FGOR, pLayerCfg->InputOffset);      
   
    /* DMA2D FGCOLR register configuration -------------------------------------*/   
    if ((pLayerCfg->InputColorMode == DMA2D_INPUT_A4) || (pLayerCfg->InputColorMode == DMA2D_INPUT_A8))
    {
      WRITE_REG(hdma2d->Instance->FGCOLR, pLayerCfg->InputAlpha & (DMA2D_FGCOLR_BLUE|DMA2D_FGCOLR_GREEN|DMA2D_FGCOLR_RED));      
    }   
  }   
  /* Initialize the DMA2D state*/
  hdma2d->State = DMA2D_STATE_READY;
}

/**
  * @brief  Configure the DMA2D CLUT Transfer.
  * @param  hdma2d:   Pointer to a DMA2D_HandleTypeDef structure that contains
  *                   the configuration information for the DMA2D.
  * @param  CLUTCfg:  Pointer to a DMA2D_CLUTCfgTypeDef structure that contains
  *                   the configuration information for the color look up table.
  * @param  LayerIdx: DMA2D Layer index.
  *                   This parameter can be one of the following values:
  *                   0(background) / 1(foreground)
  * @retval HAL status
  */
void DMA2D_ConfigCLUT(DMA2D_HandleTypeDef *hdma2d, DMA2D_CLUTCfgTypeDef CLUTCfg, unsigned int LayerIdx)
{
  /* Change DMA2D peripheral state */
  hdma2d->State = DMA2D_STATE_BUSY;
  
  /* Configure the CLUT of the background DMA2D layer */
  if(LayerIdx == 0)
  {
    /* Write background CLUT memory address */
    WRITE_REG(hdma2d->Instance->BGCMAR, (unsigned int)CLUTCfg.pCLUT);
     
    /* Write background CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->BGPFCCR, (DMA2D_BGPFCCR_CS | DMA2D_BGPFCCR_CCM), 
            ((CLUTCfg.Size << DMA2D_POSITION_BGPFCCR_CS) | (CLUTCfg.CLUTColorMode << DMA2D_POSITION_BGPFCCR_CCM)));       
 }
 /* Configure the CLUT of the foreground DMA2D layer */
 else
 {
   /* Write foreground CLUT memory address */
    WRITE_REG(hdma2d->Instance->FGCMAR, (unsigned int)CLUTCfg.pCLUT);
     
    /* Write foreground CLUT size and CLUT color mode */
    MODIFY_REG(hdma2d->Instance->FGPFCCR, (DMA2D_FGPFCCR_CS | DMA2D_FGPFCCR_CCM), 
            ((CLUTCfg.Size << DMA2D_POSITION_BGPFCCR_CS) | (CLUTCfg.CLUTColorMode << DMA2D_POSITION_FGPFCCR_CCM)));       
  }
  
  /* Set the DMA2D state to Ready*/
  hdma2d->State = DMA2D_STATE_READY;
}


/**
  * @brief  Configure the line watermark.
  * @param  hdma2d: Pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.
  * @param  Line:   Line Watermark configuration (maximum 16-bit long value expected).
  * @note   HAL_DMA2D_ProgramLineEvent() API enables the transfer watermark interrupt.
  * @note   The transfer watermark interrupt is disabled once it has occurred.
  * @retval HAL status
  */

unsigned int DMA2D_ProgramLineEvent(DMA2D_HandleTypeDef *hdma2d, unsigned int Line)
{
  if (Line > DMA2D_LWR_LW)
  {
    return 1;
  }
  else
  {      
    /* Change DMA2D peripheral state */
    hdma2d->State = DMA2D_STATE_BUSY;
  
    /* Sets the Line watermark configuration */
    WRITE_REG(hdma2d->Instance->LWR, Line);
    
    /* Enable the Line interrupt */
    __DMA2D_ENABLE_IT(hdma2d, DMA2D_IT_TW);
        
    /* Initialize the DMA2D state*/
    hdma2d->State = DMA2D_STATE_READY;
    
    return 0;
  }  
}

/**
  * @brief Enable DMA2D dead time feature.
  * @param hdma2d: DMA2D handle.
  * @retval HAL status
  */
void DMA2D_EnableDeadTime(DMA2D_HandleTypeDef *hdma2d)
{
  hdma2d->State = DMA2D_STATE_BUSY;

  /* Set DMA2D_AMTCR EN bit */
  SET_BIT(hdma2d->Instance->AMTCR, DMA2D_AMTCR_EN);

  hdma2d->State = DMA2D_STATE_READY;
}

/**
  * @brief Disable DMA2D dead time feature.
  * @param hdma2d: DMA2D handle.
  * @retval HAL status
  */
void DMA2D_DisableDeadTime(DMA2D_HandleTypeDef *hdma2d)
{
  hdma2d->State = DMA2D_STATE_BUSY;

  /* Clear DMA2D_AMTCR EN bit */
  CLEAR_BIT(hdma2d->Instance->AMTCR, DMA2D_AMTCR_EN);

  hdma2d->State = DMA2D_STATE_READY;
}

/**
  * @brief Configure dead time.
  * @note The dead time value represents the guaranteed minimum number of cycles between 
  *       two consecutive transactions on the AHB bus.
  * @param hdma2d: DMA2D handle.
  * @param DeadTime: dead time value.
  * @retval HAL status
  */
void DMA2D_ConfigDeadTime(DMA2D_HandleTypeDef *hdma2d, uint8_t DeadTime)
{
  hdma2d->State = DMA2D_STATE_BUSY;

  /* Set DMA2D_AMTCR DT field */
  MODIFY_REG(hdma2d->Instance->AMTCR, DMA2D_AMTCR_DT, (((unsigned int) DeadTime) << DMA2D_POSITION_AMTCR_DT));

  hdma2d->State = DMA2D_STATE_READY;
}

/**
  * @brief  Return the DMA2D state
  * @param  hdma2d: pointer to a DMA2D_HandleTypeDef structure that contains
  *                 the configuration information for the DMA2D.  
  * @retval HAL state
  */
DMA2D_StateTypeDef DMA2D_GetState(DMA2D_HandleTypeDef *hdma2d)
{  
  return hdma2d->State;
}

/**
  * @brief  Return the DMA2D error code
  * @param  hdma2d : pointer to a DMA2D_HandleTypeDef structure that contains
  *               the configuration information for DMA2D.
  * @retval DMA2D Error Code
  */
unsigned int DMA2D_GetError(DMA2D_HandleTypeDef *hdma2d)
{
  return hdma2d->ErrorCode;
}

/**
  * @brief  Set the DMA2D transfer parameters.
  * @param  hdma2d:     Pointer to a DMA2D_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA2D.  
  * @param  pdata:      The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  Width:      The width of data to be transferred from source to destination.
  * @param  Height:     The height of data to be transferred from source to destination.
  * @retval HAL status
  */
static void DMA2D_SetConfig(DMA2D_HandleTypeDef *hdma2d, unsigned int pdata, unsigned int DstAddress, unsigned int Width, unsigned int Height)
{  
  unsigned int tmp = 0;
  unsigned int tmp1 = 0;
  unsigned int tmp2 = 0;
  unsigned int tmp3 = 0;
  unsigned int tmp4 = 0;
    
  /* Configure DMA2D data size */
  MODIFY_REG(hdma2d->Instance->NLR, (DMA2D_NLR_NL|DMA2D_NLR_PL), (Height| (Width << DMA2D_POSITION_NLR_PL))); 
  
  /* Configure DMA2D destination address */
  WRITE_REG(hdma2d->Instance->OMAR, DstAddress);
 
  /* Register to memory DMA2D mode selected */
  if (hdma2d->Init.Mode == DMA2D_R2M)
  {    
    tmp1 = pdata & DMA2D_OCOLR_ALPHA_1;
    tmp2 = pdata & DMA2D_OCOLR_RED_1;
    tmp3 = pdata & DMA2D_OCOLR_GREEN_1;
    tmp4 = pdata & DMA2D_OCOLR_BLUE_1;
    
    /* Prepare the value to be written to the OCOLR register according to the color mode */
    if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_ARGB8888)
    {
      tmp = (tmp3 | tmp2 | tmp1| tmp4);
    }
    else if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_RGB888)
    {
      tmp = (tmp3 | tmp2 | tmp4);  
    }
    else if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_RGB565)
    {
      tmp2 = (tmp2 >> 19);
      tmp3 = (tmp3 >> 10);
      tmp4 = (tmp4 >> 3 );
      tmp  = ((tmp3 << 5) | (tmp2 << 11) | tmp4); 
    }
    else if (hdma2d->Init.ColorMode == DMA2D_OUTPUT_ARGB1555)
    { 
      tmp1 = (tmp1 >> 31);
      tmp2 = (tmp2 >> 19);
      tmp3 = (tmp3 >> 11);
      tmp4 = (tmp4 >> 3 );      
      tmp  = ((tmp3 << 5) | (tmp2 << 10) | (tmp1 << 15) | tmp4);    
    } 
    else /* Dhdma2d->Init.ColorMode = DMA2D_OUTPUT_ARGB4444 */
    {
      tmp1 = (tmp1 >> 28);
      tmp2 = (tmp2 >> 20);
      tmp3 = (tmp3 >> 12);
      tmp4 = (tmp4 >> 4 );
      tmp  = ((tmp3 << 4) | (tmp2 << 8) | (tmp1 << 12) | tmp4);
    }    
    /* Write to DMA2D OCOLR register */
    WRITE_REG(hdma2d->Instance->OCOLR, tmp);    
  } 
  else /* M2M, M2M_PFC or M2M_Blending DMA2D Mode */
  {
    /* Configure DMA2D source address */
    WRITE_REG(hdma2d->Instance->FGMAR, pdata);
  }
}
