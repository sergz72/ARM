#include "board.h"
#include "ltdc.h"

/* Private function prototypes -----------------------------------------------*/
static void LTDC_SetConfig(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, unsigned int LayerIdx);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the LTDC according to the specified
  *         parameters in the LTDC_InitTypeDef and create the associated handle.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval HAL status
  */
unsigned int LTDC_Init(LTDC_HandleTypeDef *hltdc)
{
  unsigned int tmp = 0, tmp1 = 0;

  /* Check the LTDC peripheral state */
  if(hltdc == NULL)
  {
    return 1;
  }

  if(hltdc->State == LTDC_STATE_RESET)
  {
    /* Init the low level hardware */
    LTDC_MspInit(hltdc);
  }
  
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Configures the HS, VS, DE and PC polarity */
  hltdc->Instance->GCR &= ~(LTDC_GCR_HSPOL | LTDC_GCR_VSPOL | LTDC_GCR_DEPOL | LTDC_GCR_PCPOL);
  hltdc->Instance->GCR |=  (unsigned int)(hltdc->Init.HSPolarity | hltdc->Init.VSPolarity | hltdc->Init.DEPolarity | hltdc->Init.PCPolarity);

  /* Sets Synchronization size */
  hltdc->Instance->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
  tmp = (hltdc->Init.HorizontalSync << 16);
  hltdc->Instance->SSCR |= (tmp | hltdc->Init.VerticalSync);

  /* Sets Accumulated Back porch */
  hltdc->Instance->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
  tmp = (hltdc->Init.AccumulatedHBP << 16);
  hltdc->Instance->BPCR |= (tmp | hltdc->Init.AccumulatedVBP);

  /* Sets Accumulated Active Width */
  hltdc->Instance->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
  tmp = (hltdc->Init.AccumulatedActiveW << 16);
  hltdc->Instance->AWCR |= (tmp | hltdc->Init.AccumulatedActiveH);

  /* Sets Total Width */
  hltdc->Instance->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
  tmp = (hltdc->Init.TotalWidth << 16);
  hltdc->Instance->TWCR |= (tmp | hltdc->Init.TotalHeigh);

  /* Sets the background color value */
  tmp = ((unsigned int)(hltdc->Init.Backcolor.Green) << 8U);
  tmp1 = ((unsigned int)(hltdc->Init.Backcolor.Red) << 16U);
  hltdc->Instance->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
  hltdc->Instance->BCCR |= (tmp1 | tmp | hltdc->Init.Backcolor.Blue);

  /* Enable the transfer Error interrupt */
  __LTDC_ENABLE_IT(hltdc, LTDC_IT_TE);

  /* Enable the FIFO underrun interrupt */
  __LTDC_ENABLE_IT(hltdc, LTDC_IT_FU);

  /* Enable LTDC by setting LTDCEN bit */
  __LTDC_ENABLE(hltdc);

  /* Initialize the error code */
  hltdc->ErrorCode = LTDC_ERROR_NONE;

  /* Initialize the LTDC state*/
  hltdc->State = LTDC_STATE_READY;

  return 0;
}

/**
  * @brief  Deinitializes the LTDC peripheral registers to their default reset
  *         values.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */

void LTDC_DeInit(LTDC_HandleTypeDef *hltdc)
{
  /* DeInit the low level hardware */
  LTDC_MspDeInit(hltdc);

  /* Initialize the error code */
  hltdc->ErrorCode = LTDC_ERROR_NONE;

  /* Initialize the LTDC state*/
  hltdc->State = LTDC_STATE_RESET;
}

/**
  * @brief  Initializes the LTDC MSP.
  * @param  hltdc : pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
}

/**
  * @brief  DeInitializes the LTDC MSP.
  * @param  hltdc : pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
}

/**
  * @brief  Handles LTDC interrupt request.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.  
  * @retval HAL status
  */
void _LTDC_IRQHandler(LTDC_HandleTypeDef *hltdc)
{
  /* Transfer Error Interrupt management ***************************************/
  if(__LTDC_GET_FLAG(hltdc, LTDC_FLAG_TE) != RESET)
  {
    if(__LTDC_GET_IT_SOURCE(hltdc, LTDC_IT_TE) != RESET)
    {
      /* Disable the transfer Error interrupt */
      __LTDC_DISABLE_IT(hltdc, LTDC_IT_TE);

      /* Clear the transfer error flag */
      __LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_TE);

      /* Update error code */
      hltdc->ErrorCode |= LTDC_ERROR_TE;

      /* Change LTDC state */
      hltdc->State = LTDC_STATE_ERROR;

      /* Transfer error Callback */
      LTDC_ErrorCallback(hltdc);
    }
  }
  /* FIFO underrun Interrupt management ***************************************/
  if(__LTDC_GET_FLAG(hltdc, LTDC_FLAG_FU) != RESET)
  {
    if(__LTDC_GET_IT_SOURCE(hltdc, LTDC_IT_FU) != RESET)
    {
      /* Disable the FIFO underrun interrupt */
      __LTDC_DISABLE_IT(hltdc, LTDC_IT_FU);

      /* Clear the FIFO underrun flag */
      __LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_FU);

      /* Update error code */
      hltdc->ErrorCode |= LTDC_ERROR_FU;

      /* Change LTDC state */
      hltdc->State = LTDC_STATE_ERROR;

      /* Transfer error Callback */
      LTDC_ErrorCallback(hltdc);
    }
  }
  /* Line Interrupt management ************************************************/
  if(__LTDC_GET_FLAG(hltdc, LTDC_FLAG_LI) != RESET)
  {
    if(__LTDC_GET_IT_SOURCE(hltdc, LTDC_IT_LI) != RESET)
    {
      /* Disable the Line interrupt */
      __LTDC_DISABLE_IT(hltdc, LTDC_IT_LI);

      /* Clear the Line interrupt flag */  
      __LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_LI);

      /* Change LTDC state */
      hltdc->State = LTDC_STATE_READY;

      /* Line interrupt Callback */
      LTDC_LineEvenCallback(hltdc);
    }
  }
  /* Register reload Interrupt management ***************************************/
  if(__LTDC_GET_FLAG(hltdc, LTDC_FLAG_RR) != RESET)
  {
    if(__LTDC_GET_IT_SOURCE(hltdc, LTDC_IT_RR) != RESET)
    {
      /* Disable the register reload interrupt */
      __LTDC_DISABLE_IT(hltdc, LTDC_IT_RR);
      
      /* Clear the register reload flag */
      __LTDC_CLEAR_FLAG(hltdc, LTDC_FLAG_RR);
      
      /* Change LTDC state */
      hltdc->State = LTDC_STATE_READY;
      
      /* Register reload interrupt Callback */
      LTDC_ReloadEventCallback(hltdc);
    }
  }  
}

/**
  * @brief  Error LTDC callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void LTDC_ErrorCallback(LTDC_HandleTypeDef *hltdc)
{
}

/**
  * @brief  Line Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{
}

/**
  * @brief  Reload Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval None
  */
__weak void LTDC_ReloadEventCallback(LTDC_HandleTypeDef *hltdc)
{
}

/**
  * @brief  Configure the LTDC Layer according to the specified
  *         parameters in the LTDC_InitTypeDef and create the associated handle.
  * @param  hltdc:     pointer to a LTDC_HandleTypeDef structure that contains
  *                    the configuration information for the LTDC.
  * @param  pLayerCfg: pointer to a LTDC_LayerCfgTypeDef structure that contains
  *                    the configuration information for the Layer.
  * @param  LayerIdx:  LTDC Layer index.
  *                    This parameter can be one of the following values:
  *                    0 or 1
  * @retval HAL status
  */
void LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, unsigned int LayerIdx)
{   
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Copy new layer configuration into handle structure */
  hltdc->LayerCfg[LayerIdx] = *pLayerCfg;  

  /* Configure the LTDC Layer */  
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Initialize the LTDC state*/
  hltdc->State  = LTDC_STATE_READY;
}

/**
  * @brief  Configure the color keying.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  RGBValue: the color key value
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval HAL status
  */
void LTDC_ConfigColorKeying(LTDC_HandleTypeDef *hltdc, unsigned int RGBValue, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Configures the default color values */
  LTDC_LAYER(hltdc, LayerIdx)->CKCR &=  ~(LTDC_LxCKCR_CKBLUE | LTDC_LxCKCR_CKGREEN | LTDC_LxCKCR_CKRED);
  LTDC_LAYER(hltdc, LayerIdx)->CKCR  = RGBValue;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Load the color lookup table.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  pCLUT:    pointer to the color lookup table address.
  * @param  CLUTSize: the color lookup table size.  
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval HAL status
  */
void LTDC_ConfigCLUT(LTDC_HandleTypeDef *hltdc, unsigned int *pCLUT, unsigned int CLUTSize, unsigned int LayerIdx)
{
  unsigned int tmp = 0;
  unsigned int counter = 0;
  unsigned int pcounter = 0;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  for(counter = 0; (counter < CLUTSize); counter++)
  {
    if(hltdc->LayerCfg[LayerIdx].PixelFormat == LTDC_PIXEL_FORMAT_AL44)
    {
      tmp  = (((counter + 16*counter) << 24) | ((unsigned int)(*pCLUT) & 0xFF) | ((unsigned int)(*pCLUT) & 0xFF00) | ((unsigned int)(*pCLUT) & 0xFF0000));
    }
    else
    { 
      tmp  = ((counter << 24) | ((unsigned int)(*pCLUT) & 0xFF) | ((unsigned int)(*pCLUT) & 0xFF00) | ((unsigned int)(*pCLUT) & 0xFF0000));
    }
    pcounter = (unsigned int)pCLUT + sizeof(*pCLUT);
    pCLUT = (unsigned int *)pcounter;

    /* Specifies the C-LUT address and RGB value */
    LTDC_LAYER(hltdc, LayerIdx)->CLUTWR  = tmp;
  }
  
  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Enable the color keying.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_EnableColorKeying(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{  
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Enable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (unsigned int)LTDC_LxCR_COLKEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}
  
/**
  * @brief  Disable the color keying.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_DisableColorKeying(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Disable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(unsigned int)LTDC_LxCR_COLKEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Enable the color lookup table.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_EnableCLUT(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (unsigned int)LTDC_LxCR_CLUTEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Disable the color lookup table.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1   
  * @retval  HAL status
  */
void LTDC_DisableCLUT(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(unsigned int)LTDC_LxCR_CLUTEN;

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Enables Dither.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

void LTDC_EnableDither(LTDC_HandleTypeDef *hltdc)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Enable Dither by setting DTEN bit */
  LTDC->GCR |= (unsigned int)LTDC_GCR_DEN;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Disables Dither.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval  HAL status
  */

void LTDC_DisableDither(LTDC_HandleTypeDef *hltdc)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Disable Dither by setting DTEN bit */
  LTDC->GCR &= ~(unsigned int)LTDC_GCR_DEN;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Set the LTDC window size.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  XSize:    LTDC Pixel per line
  * @param  YSize:    LTDC Line number
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_SetWindowSize(LTDC_HandleTypeDef *hltdc, unsigned int XSize, unsigned int YSize, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* update horizontal start/stop */
  pLayerCfg->WindowX0 = 0;
  pLayerCfg->WindowX1 = XSize + pLayerCfg->WindowX0;

  /* update vertical start/stop */  
  pLayerCfg->WindowY0 = 0;
  pLayerCfg->WindowY1 = YSize + pLayerCfg->WindowY0;

  /* Reconfigures the color frame buffer pitch in byte */
  pLayerCfg->ImageWidth = XSize;

  /* Reconfigures the frame buffer line number */
  pLayerCfg->ImageHeight = YSize;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Set the LTDC window position.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  X0:       LTDC window X offset
  * @param  Y0:       LTDC window Y offset
  * @param  LayerIdx:  LTDC Layer index.
  *                         This parameter can be one of the following values:
  *                         0 or 1
  * @retval  HAL status
  */
void LTDC_SetWindowPosition(LTDC_HandleTypeDef *hltdc, unsigned int X0, unsigned int Y0, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;
  
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* update horizontal start/stop */
  pLayerCfg->WindowX0 = X0;
  pLayerCfg->WindowX1 = X0 + pLayerCfg->ImageWidth;

  /* update vertical start/stop */
  pLayerCfg->WindowY0 = Y0;
  pLayerCfg->WindowY1 = Y0 + pLayerCfg->ImageHeight;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Reconfigure the pixel format.
  * @param  hltdc:       pointer to a LTDC_HandleTypeDef structure that contains
  *                      the configuration information for the LTDC.
  * @param  Pixelformat: new pixel format value.
  * @param  LayerIdx:    LTDC Layer index.
  *                      This parameter can be one of the following values:
  *                      0 or 1.
  * @retval  HAL status
  */
void LTDC_SetPixelFormat(LTDC_HandleTypeDef *hltdc, unsigned int Pixelformat, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];  

  /* Reconfigure the pixel format */
  pLayerCfg->PixelFormat = Pixelformat;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);   

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Reconfigure the layer alpha value.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Alpha:    new alpha value.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_SetAlpha(LTDC_HandleTypeDef *hltdc, unsigned int Alpha, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Reconfigure the Alpha value */
  pLayerCfg->Alpha = Alpha;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Reconfigure the frame buffer Address.
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Address:  new address value.
  * @param  LayerIdx: LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1.
  * @retval  HAL status
  */
void LTDC_SetAddress(LTDC_HandleTypeDef *hltdc, unsigned int Address, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Reconfigure the Address */
  pLayerCfg->FBStartAdress = Address;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Sets the Reload type */
  hltdc->Instance->SRCR = LTDC_SRCR_IMR;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Function used to reconfigure the pitch for specific cases where the attached LayerIdx buffer have a width that is
  *         larger than the one intended to be displayed on screen. Example of a buffer 800x480 attached to layer for which we 
  *         want to read and display on screen only a portion 320x240 taken in the center of the buffer. The pitch in pixels 
  *         will be in that case 800 pixels and not 320 pixels as initially configured by previous call to HAL_LTDC_ConfigLayer().
  *         Note : this function should be called only after a previous call to HAL_LTDC_ConfigLayer() to modify the default pitch
  *                configured by HAL_LTDC_ConfigLayer() when required (refer to example described just above).
  * @param  hltdc:             pointer to a LTDC_HandleTypeDef structure that contains
  *                            the configuration information for the LTDC.
  * @param  LinePitchInPixels: New line pitch in pixels to configure for LTDC layer 'LayerIdx'.
  * @param  LayerIdx:          LTDC layer index concerned by the modification of line pitch.
  * @retval HAL status
  */
void LTDC_SetPitch(LTDC_HandleTypeDef *hltdc, unsigned int LinePitchInPixels, unsigned int LayerIdx)
{
  unsigned int tmp = 0;
  unsigned int pitchUpdate = 0;
  unsigned int pixelFormat = 0;
  
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;
  
  /* get LayerIdx used pixel format */
  pixelFormat = hltdc->LayerCfg[LayerIdx].PixelFormat;
  
  if(pixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    tmp = 4;
  }
  else if (pixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    tmp = 3;
  }
  else if((pixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (pixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
          (pixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
         (pixelFormat == LTDC_PIXEL_FORMAT_AL88))
  {
    tmp = 2;
  }
  else
  {
    tmp = 1;
  }
  
  pitchUpdate = ((LinePitchInPixels * tmp) << 16);
  
  /* Clear previously set standard pitch */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR &= ~LTDC_LxCFBLR_CFBP;
  
  /* Sets the Reload type as immediate update of LTDC pitch configured above */
  LTDC->SRCR |= LTDC_SRCR_IMR;
  
  /* Set new line pitch value */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR |= pitchUpdate;
  
  /* Sets the Reload type as immediate update of LTDC pitch configured above */
  LTDC->SRCR |= LTDC_SRCR_IMR;
  
  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Define the position of the line interrupt.
  * @param  hltdc:             pointer to a LTDC_HandleTypeDef structure that contains
  *                            the configuration information for the LTDC.
  * @param  Line:   Line Interrupt Position.
  * @retval  HAL status
  */
void LTDC_ProgramLineEvent(LTDC_HandleTypeDef *hltdc, unsigned int Line)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Enable the Line interrupt */
  __LTDC_ENABLE_IT(hltdc, LTDC_IT_LI);

  /* Sets the Line Interrupt position */
  LTDC->LIPCR = (unsigned int)Line;

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  LTDC configuration reload.
  * @param  hltdc:            pointer to a LTDC_HandleTypeDef structure that contains
  *                           the configuration information for the LTDC.
  * @param  ReloadType:       This parameter can be one of the following values :
  *                           LTDC_RELOAD_IMMEDIATE : Immediate Reload
  *                           LTDC_RELOAD_VERTICAL_BLANKING  : Reload in the next Vertical Blanking
  * @retval  HAL status
  */
void LTDC_Reload(LTDC_HandleTypeDef *hltdc, unsigned int ReloadType)
{
  /* Enable the Reload interrupt */
  __LTDC_ENABLE_IT(hltdc, LTDC_IT_RR);
       
  /* Apply Reload type */
  hltdc->Instance->SRCR = ReloadType;        

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Configure the LTDC Layer according to the specified without reloading
  *         parameters in the LTDC_InitTypeDef and create the associated handle.
  *         Variant of the function HAL_LTDC_ConfigLayer without immediate reload
  * @param  hltdc:     pointer to a LTDC_HandleTypeDef structure that contains
  *                    the configuration information for the LTDC.
  * @param  pLayerCfg: pointer to a LTDC_LayerCfgTypeDef structure that contains
  *                    the configuration information for the Layer.
  * @param  LayerIdx:  LTDC Layer index.
  *                    This parameter can be one of the following values:
  *                    0 or 1
  * @retval HAL status
  */
void LTDC_ConfigLayer_NoReload(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, unsigned int LayerIdx)
{   
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Copy new layer configuration into handle structure */
  hltdc->LayerCfg[LayerIdx] = *pLayerCfg;  

  /* Configure the LTDC Layer */  
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Do not Sets the Reload  */

  /* Initialize the LTDC state*/
  hltdc->State  = LTDC_STATE_READY;
}

/**
  * @brief  Set the LTDC window size without reloading.
  *         Variant of the function HAL_LTDC_SetWindowSize without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  XSize:    LTDC Pixel per line
  * @param  YSize:    LTDC Line number
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_SetWindowSize_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int XSize, unsigned int YSize, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* update horizontal start/stop */
  pLayerCfg->WindowX0 = 0;
  pLayerCfg->WindowX1 = XSize + pLayerCfg->WindowX0;

  /* update vertical start/stop */  
  pLayerCfg->WindowY0 = 0;
  pLayerCfg->WindowY1 = YSize + pLayerCfg->WindowY0;

  /* Reconfigures the color frame buffer pitch in byte */
  pLayerCfg->ImageWidth = XSize;

  /* Reconfigures the frame buffer line number */
  pLayerCfg->ImageHeight = YSize;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Set the LTDC window position without reloading.
  *         Variant of the function HAL_LTDC_SetWindowPosition without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  X0:       LTDC window X offset
  * @param  Y0:       LTDC window Y offset
  * @param  LayerIdx:  LTDC Layer index.
  *                         This parameter can be one of the following values:
  *                         0 or 1
  * @retval  HAL status
  */
void LTDC_SetWindowPosition_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int X0, unsigned int Y0, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;
  
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* update horizontal start/stop */
  pLayerCfg->WindowX0 = X0;
  pLayerCfg->WindowX1 = X0 + pLayerCfg->ImageWidth;

  /* update vertical start/stop */
  pLayerCfg->WindowY0 = Y0;
  pLayerCfg->WindowY1 = Y0 + pLayerCfg->ImageHeight;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Reconfigure the pixel format without reloading.
  *         Variant of the function HAL_LTDC_SetPixelFormat without immediate reload
  * @param  hltdc:       pointer to a LTDC_HandleTypeDfef structure that contains
  *                      the configuration information for the LTDC.
  * @param  Pixelformat: new pixel format value.
  * @param  LayerIdx:    LTDC Layer index.
  *                      This parameter can be one of the following values:
  *                      0 or 1.
  * @retval  HAL status
  */
void LTDC_SetPixelFormat_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int Pixelformat, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];  

  /* Reconfigure the pixel format */
  pLayerCfg->PixelFormat = Pixelformat;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);   

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Reconfigure the layer alpha value without reloading.
  *         Variant of the function HAL_LTDC_SetAlpha without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Alpha:    new alpha value.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_SetAlpha_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int Alpha, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Reconfigure the Alpha value */
  pLayerCfg->Alpha = Alpha;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Reconfigure the frame buffer Address without reloading.
  *         Variant of the function HAL_LTDC_SetAddress without immediate reload   
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  Address:  new address value.
  * @param  LayerIdx: LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1.
  * @retval  HAL status
  */
void LTDC_SetAddress_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int Address, unsigned int LayerIdx)
{
  LTDC_LayerCfgTypeDef *pLayerCfg;

  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Get layer configuration from handle structure */
  pLayerCfg = &hltdc->LayerCfg[LayerIdx];

  /* Reconfigure the Address */
  pLayerCfg->FBStartAdress = Address;

  /* Set LTDC parameters */
  LTDC_SetConfig(hltdc, pLayerCfg, LayerIdx);

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Function used to reconfigure the pitch for specific cases where the attached LayerIdx buffer have a width that is
  *         larger than the one intended to be displayed on screen. Example of a buffer 800x480 attached to layer for which we 
  *         want to read and display on screen only a portion 320x240 taken in the center of the buffer. The pitch in pixels 
  *         will be in that case 800 pixels and not 320 pixels as initially configured by previous call to HAL_LTDC_ConfigLayer().
  *         Note : this function should be called only after a previous call to HAL_LTDC_ConfigLayer() to modify the default pitch
  *                configured by HAL_LTDC_ConfigLayer() when required (refer to example described just above).
  *         Variant of the function HAL_LTDC_SetPitch without immediate reload    
  * @param  hltdc:             pointer to a LTDC_HandleTypeDef structure that contains
  *                            the configuration information for the LTDC.
  * @param  LinePitchInPixels: New line pitch in pixels to configure for LTDC layer 'LayerIdx'.
  * @param  LayerIdx:          LTDC layer index concerned by the modification of line pitch.
  * @retval HAL status
  */
void LTDC_SetPitch_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LinePitchInPixels, unsigned int LayerIdx)
{
  unsigned int tmp = 0;
  unsigned int pitchUpdate = 0;
  unsigned int pixelFormat = 0;
  
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;
  
  /* get LayerIdx used pixel format */
  pixelFormat = hltdc->LayerCfg[LayerIdx].PixelFormat;
  
  if(pixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    tmp = 4;
  }
  else if (pixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    tmp = 3;
  }
  else if((pixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
          (pixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
          (pixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
         (pixelFormat == LTDC_PIXEL_FORMAT_AL88))
  {
    tmp = 2;
  }
  else
  {
    tmp = 1;
  }
  
  pitchUpdate = ((LinePitchInPixels * tmp) << 16);
  
  /* Clear previously set standard pitch */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR &= ~LTDC_LxCFBLR_CFBP;
  
  /* Set new line pitch value */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR |= pitchUpdate;
  
  /* Do not Sets the Reload  */
  
  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}


/**
  * @brief  Configure the color keying without reloading.
  *         Variant of the function HAL_LTDC_ConfigColorKeying without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  RGBValue: the color key value
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval HAL status
  */
void LTDC_ConfigColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int RGBValue, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Configures the default color values */
  LTDC_LAYER(hltdc, LayerIdx)->CKCR &=  ~(LTDC_LxCKCR_CKBLUE | LTDC_LxCKCR_CKGREEN | LTDC_LxCKCR_CKRED);
  LTDC_LAYER(hltdc, LayerIdx)->CKCR  = RGBValue;

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Enable the color keying without reloading.
  *         Variant of the function HAL_LTDC_EnableColorKeying without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_EnableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{  
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Enable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (unsigned int)LTDC_LxCR_COLKEN;

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Disable the color keying without reloading.
  *         Variant of the function HAL_LTDC_DisableColorKeying without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_DisableColorKeying_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Disable LTDC color keying by setting COLKEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(unsigned int)LTDC_LxCR_COLKEN;

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Enable the color lookup table without reloading.
  *         Variant of the function HAL_LTDC_EnableCLUT without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1
  * @retval  HAL status
  */
void LTDC_EnableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (unsigned int)LTDC_LxCR_CLUTEN;

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @brief  Disable the color lookup table without reloading.
  *         Variant of the function HAL_LTDC_DisableCLUT without immediate reload
  * @param  hltdc:    pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  LayerIdx:  LTDC Layer index.
  *                   This parameter can be one of the following values:
  *                   0 or 1   
  * @retval  HAL status
  */
void LTDC_DisableCLUT_NoReload(LTDC_HandleTypeDef *hltdc, unsigned int LayerIdx)
{
  /* Change LTDC peripheral state */
  hltdc->State = LTDC_STATE_BUSY;

  /* Disable LTDC color lookup table by setting CLUTEN bit */
  LTDC_LAYER(hltdc, LayerIdx)->CR &= ~(unsigned int)LTDC_LxCR_CLUTEN;

  /* Do not Sets the Reload  */

  /* Change the LTDC state*/
  hltdc->State = LTDC_STATE_READY;
}

/**
  * @}
  */

/** @defgroup LTDC_Exported_Functions_Group4 Peripheral State and Errors functions
 *  @brief    Peripheral State and Errors functions 
 *
@verbatim   
 ===============================================================================
                  ##### Peripheral State and Errors functions #####
 ===============================================================================  
    [..]
    This subsection provides functions allowing to
      (+) Check the LTDC state.
      (+) Get error code.  

@endverbatim
  * @{
  */ 

/**
  * @brief  Return the LTDC state
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @retval HAL state
  */
LTDC_StateTypeDef LTDC_GetState(LTDC_HandleTypeDef *hltdc)
{
  return hltdc->State;
}

/**
* @brief  Return the LTDC error code
* @param  hltdc : pointer to a LTDC_HandleTypeDef structure that contains
  *               the configuration information for the LTDC.
* @retval LTDC Error Code
*/
unsigned int LTDC_GetError(LTDC_HandleTypeDef *hltdc)
{
  return hltdc->ErrorCode;
}

/**
  * @}
  */

/**
  * @brief  Configures the LTDC peripheral 
  * @param  hltdc   :  Pointer to a LTDC_HandleTypeDef structure that contains
  *                   the configuration information for the LTDC.
  * @param  pLayerCfg: Pointer LTDC Layer Configuration structure
  * @param  LayerIdx:  LTDC Layer index.
  *                    This parameter can be one of the following values: 0 or 1
  * @retval None
  */
static void LTDC_SetConfig(LTDC_HandleTypeDef *hltdc, LTDC_LayerCfgTypeDef *pLayerCfg, unsigned int LayerIdx)
{
  unsigned int tmp = 0;
  unsigned int tmp1 = 0;
  unsigned int tmp2 = 0;

  /* Configures the horizontal start and stop position */
  tmp = ((pLayerCfg->WindowX1 + ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16)) << 16);
  LTDC_LAYER(hltdc, LayerIdx)->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
  LTDC_LAYER(hltdc, LayerIdx)->WHPCR = ((pLayerCfg->WindowX0 + ((hltdc->Instance->BPCR & LTDC_BPCR_AHBP) >> 16) + 1) | tmp);

  /* Configures the vertical start and stop position */
  tmp = ((pLayerCfg->WindowY1 + (hltdc->Instance->BPCR & LTDC_BPCR_AVBP)) << 16);
  LTDC_LAYER(hltdc, LayerIdx)->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
  LTDC_LAYER(hltdc, LayerIdx)->WVPCR  = ((pLayerCfg->WindowY0 + (hltdc->Instance->BPCR & LTDC_BPCR_AVBP) + 1) | tmp);  

  /* Specifies the pixel format */
  LTDC_LAYER(hltdc, LayerIdx)->PFCR &= ~(LTDC_LxPFCR_PF);
  LTDC_LAYER(hltdc, LayerIdx)->PFCR = (pLayerCfg->PixelFormat);

  /* Configures the default color values */
  tmp = ((unsigned int)(pLayerCfg->Backcolor.Green) << 8);
  tmp1 = ((unsigned int)(pLayerCfg->Backcolor.Red) << 16);
  tmp2 = (pLayerCfg->Alpha0 << 24);  
  LTDC_LAYER(hltdc, LayerIdx)->DCCR &= ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
  LTDC_LAYER(hltdc, LayerIdx)->DCCR = (pLayerCfg->Backcolor.Blue | tmp | tmp1 | tmp2); 

  /* Specifies the constant alpha value */
  LTDC_LAYER(hltdc, LayerIdx)->CACR &= ~(LTDC_LxCACR_CONSTA);
  LTDC_LAYER(hltdc, LayerIdx)->CACR = (pLayerCfg->Alpha);

  /* Specifies the blending factors */
  LTDC_LAYER(hltdc, LayerIdx)->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
  LTDC_LAYER(hltdc, LayerIdx)->BFCR = (pLayerCfg->BlendingFactor1 | pLayerCfg->BlendingFactor2);

  /* Configures the color frame buffer start address */
  LTDC_LAYER(hltdc, LayerIdx)->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
  LTDC_LAYER(hltdc, LayerIdx)->CFBAR = (pLayerCfg->FBStartAdress);

  if(pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB8888)
  {
    tmp = 4;
  }
  else if (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB888)
  {
    tmp = 3;
  }
  else if((pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB4444) || \
    (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_RGB565)   || \
      (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_ARGB1555) || \
        (pLayerCfg->PixelFormat == LTDC_PIXEL_FORMAT_AL88))
  {
    tmp = 2;
  }
  else
  {
    tmp = 1;
  }

  /* Configures the color frame buffer pitch in byte */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR  &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
  LTDC_LAYER(hltdc, LayerIdx)->CFBLR  = (((pLayerCfg->ImageWidth * tmp) << 16) | (((pLayerCfg->WindowX1 - pLayerCfg->WindowX0) * tmp)  + 3));

  /* Configures the frame buffer line number */
  LTDC_LAYER(hltdc, LayerIdx)->CFBLNR  &= ~(LTDC_LxCFBLNR_CFBLNBR);
  LTDC_LAYER(hltdc, LayerIdx)->CFBLNR  = (pLayerCfg->ImageHeight);

  /* Enable LTDC_Layer by setting LEN bit */  
  LTDC_LAYER(hltdc, LayerIdx)->CR |= (unsigned int)LTDC_LxCR_LEN;
}

#if defined (STM32F769xx) || defined (STM32F779xx)
/**
  * @brief  Retrieve common parameters from DSI Video mode configuration structure
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @param  VidCfg: pointer to a DSI_VidCfgTypeDef structure that contains
  *                 the DSI video mode configuration parameters
  * @note   The implementation of this function is taking into account the LTDC
  *         polarities inversion as described in the current LTDC specification
  * @retval HAL status
  */
void LTDC_StructInitFromVideoConfig(LTDC_HandleTypeDef* hltdc, DSI_VidCfgTypeDef *VidCfg)
{
  /* Retrieve signal polarities from DSI */
  
  /* The following polarities are inverted:
                     LTDC_DEPOLARITY_AL <-> LTDC_DEPOLARITY_AH
	                   LTDC_VSPOLARITY_AL <-> LTDC_VSPOLARITY_AH
	                   LTDC_HSPOLARITY_AL <-> LTDC_HSPOLARITY_AH)*/
  
  /* Note 1 : Code in line w/ Current LTDC specification */
  hltdc->Init.DEPolarity = (VidCfg->DEPolarity == DSI_DATA_ENABLE_ACTIVE_HIGH) ? LTDC_DEPOLARITY_AL : LTDC_DEPOLARITY_AH;
  hltdc->Init.VSPolarity = (VidCfg->VSPolarity == DSI_VSYNC_ACTIVE_HIGH) ? LTDC_VSPOLARITY_AL : LTDC_VSPOLARITY_AH;
  hltdc->Init.HSPolarity = (VidCfg->HSPolarity == DSI_HSYNC_ACTIVE_HIGH) ? LTDC_HSPOLARITY_AL : LTDC_HSPOLARITY_AH;

  /* Note 2: Code to be used in case LTDC polarities inversion updated in the specification */
  /* hltdc->Init.DEPolarity = VidCfg->DEPolarity << 29;
     hltdc->Init.VSPolarity = VidCfg->VSPolarity << 29;
     hltdc->Init.HSPolarity = VidCfg->HSPolarity << 29; */
    
  /* Retrieve vertical timing parameters from DSI */
  hltdc->Init.VerticalSync       = VidCfg->VerticalSyncActive - 1;
  hltdc->Init.AccumulatedVBP     = VidCfg->VerticalSyncActive + VidCfg->VerticalBackPorch - 1;
  hltdc->Init.AccumulatedActiveH = VidCfg->VerticalSyncActive + VidCfg->VerticalBackPorch + VidCfg->VerticalActive - 1;
  hltdc->Init.TotalHeigh         = VidCfg->VerticalSyncActive + VidCfg->VerticalBackPorch + VidCfg->VerticalActive + VidCfg->VerticalFrontPorch - 1;
}

/**
  * @brief  Retrieve common parameters from DSI Adapted command mode configuration structure
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the LTDC.
  * @param  CmdCfg: pointer to a DSI_CmdCfgTypeDef structure that contains
  *                 the DSI command mode configuration parameters
  * @note   The implementation of this function is taking into account the LTDC
  *         polarities inversion as described in the current LTDC specification
  * @retval HAL status
  */
void LTDC_StructInitFromAdaptedCommandConfig(LTDC_HandleTypeDef* hltdc, DSI_CmdCfgTypeDef *CmdCfg)
{
  /* Retrieve signal polarities from DSI */
  
  /* The following polarities are inverted:
                     LTDC_DEPOLARITY_AL <-> LTDC_DEPOLARITY_AH
	                   LTDC_VSPOLARITY_AL <-> LTDC_VSPOLARITY_AH
	                   LTDC_HSPOLARITY_AL <-> LTDC_HSPOLARITY_AH)*/
  
  /* Note 1 : Code in line w/ Current LTDC specification */
  hltdc->Init.DEPolarity = (CmdCfg->DEPolarity == DSI_DATA_ENABLE_ACTIVE_HIGH) ? LTDC_DEPOLARITY_AL : LTDC_DEPOLARITY_AH;
  hltdc->Init.VSPolarity = (CmdCfg->VSPolarity == DSI_VSYNC_ACTIVE_HIGH) ? LTDC_VSPOLARITY_AL : LTDC_VSPOLARITY_AH;
  hltdc->Init.HSPolarity = (CmdCfg->HSPolarity == DSI_HSYNC_ACTIVE_HIGH) ? LTDC_HSPOLARITY_AL : LTDC_HSPOLARITY_AH;
  
  /* Note 2: Code to be used in case LTDC polarities inversion updated in the specification */
  /* hltdc->Init.DEPolarity = CmdCfg->DEPolarity << 29;
     hltdc->Init.VSPolarity = CmdCfg->VSPolarity << 29;
     hltdc->Init.HSPolarity = CmdCfg->HSPolarity << 29; */
}
#endif /*STM32F769xx | STM32F779xx */
