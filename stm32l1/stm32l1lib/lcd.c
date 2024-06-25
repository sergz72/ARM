#include "board.h"
#include "lcd.h"

/* Private define ------------------------------------------------------------*/
/* ------------ LCD registers bit address in the alias region --------------- */
#define LCD_OFFSET                   (LCD_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of LCDEN bit */
#define CR_OFFSET                    (LCD_OFFSET + 0x00)
#define LCDEN_BitNumber              0x00
#define CR_LCDEN_BB                  (PERIPH_BB_BASE + (CR_OFFSET * 32) + (LCDEN_BitNumber * 4))

/* Alias word address of MUX_SEG bit */
#define MUX_SEG_BitNumber            0x07
#define CR_MUX_SEG_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (MUX_SEG_BitNumber * 4))


/* --- FCR Register ---*/

/* Alias word address of HD bit */
#define FCR_OFFSET                   (LCD_OFFSET + 0x04)
#define HD_BitNumber                 0x00
#define FCR_HD_BB                    (PERIPH_BB_BASE + (FCR_OFFSET * 32) + (HD_BitNumber * 4))

/* --- SR Register ---*/

/* Alias word address of UDR bit */
#define SR_OFFSET                    (LCD_OFFSET + 0x08)
#define UDR_BitNumber                0x02
#define SR_UDR_BB                    (PERIPH_BB_BASE + (SR_OFFSET * 32) + (UDR_BitNumber * 4))

#define FCR_MASK                     ((uint32_t)0xFC03FFFF)  /* LCD FCR Mask */
#define CR_MASK                      ((uint32_t)0xFFFFFF81)  /* LCD CR Mask */
#define PON_MASK                     ((uint32_t)0xFFFFFF8F)  /* LCD PON Mask */
#define DEAD_MASK                    ((uint32_t)0xFFFFFC7F)  /* LCD DEAD Mask */
#define BLINK_MASK                   ((uint32_t)0xFFFC1FFF)  /* LCD BLINK Mask */
#define CONTRAST_MASK                ((uint32_t)0xFFFFE3FF)  /* LCD CONTRAST Mask */

#define SYNCHRO_TIMEOUT          ((uint32_t) 0x00008000)

/**
  * @brief  Initializes the LCD peripheral according to the specified parameters
  *         in the LCD_InitStruct.
  * @note   This function can be used only when the LCD is disabled.
  * @param  LCD_InitStruct: pointer to a LCD_InitTypeDef structure that contains
  *         the configuration information for the specified LCD peripheral.
  * @retval None
  */
void LCD_Init(LCD_InitTypeDef* LCD_InitStruct)
{
  LCD->FCR &= (uint32_t)FCR_MASK;
  LCD->FCR |= (uint32_t)(LCD_InitStruct->LCD_Prescaler | LCD_InitStruct->LCD_Divider);

  LCD_WaitForSynchro();

  LCD->CR &= (uint32_t)CR_MASK;
  LCD->CR |= (uint32_t)(LCD_InitStruct->LCD_Duty | LCD_InitStruct->LCD_Bias | \
                        LCD_InitStruct->LCD_VoltageSource);
}

/**
  * @brief  Fills each LCD_InitStruct member with its default value.
  * @param  LCD_InitStruct: pointer to a LCD_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void LCD_StructInit(LCD_InitTypeDef* LCD_InitStruct)
{
/*--------------- Reset LCD init structure parameters values -----------------*/
  LCD_InitStruct->LCD_Prescaler = LCD_Prescaler_1; /*!< Initialize the LCD_Prescaler member */
  
  LCD_InitStruct->LCD_Divider = LCD_Divider_16; /*!< Initialize the LCD_Divider member */
  
  LCD_InitStruct->LCD_Duty = LCD_Duty_Static; /*!< Initialize the LCD_Duty member */
  
  LCD_InitStruct->LCD_Bias = LCD_Bias_1_4; /*!< Initialize the LCD_Bias member */
  
  LCD_InitStruct->LCD_VoltageSource = LCD_VoltageSource_Internal; /*!< Initialize the LCD_VoltageSource member */
}

/**
  * @brief  Enables or disables the LCD Controller.
  * @param  NewState: new state of the LCD peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LCD_Cmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CR_LCDEN_BB = (uint32_t)NewState;
}

/**
  * @brief  Waits until the LCD FCR register is synchronized in the LCDCLK domain.
  *   This function must be called after any write operation to LCD_FCR register.
  * @param  None
  * @retval None
  */
void LCD_WaitForSynchro(void)
{
  uint32_t synchrocounter = 0;
  uint32_t synchrostatus = 0x00;
  
  /* Loop until FCRSF flag is set */
  do
  {
    synchrostatus = LCD->SR & LCD_FLAG_FCRSF;
    synchrocounter++;  
  } while((synchrocounter != SYNCHRO_TIMEOUT) && (synchrostatus == 0x00));
}

/**
  * @brief  Enables or disables the low resistance divider. Displays with high
  *         internal resistance may need a longer drive time to achieve
  *         satisfactory contrast. This function is useful in this case if some
  *         additional power consumption can be tolerated.
  * @note   When this mode is enabled, the PulseOn Duration (PON) have to be 
  *         programmed to 1/CK_PS (LCD_PulseOnDuration_1).
  * @param  NewState: new state of the low resistance divider.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LCD_HighDriveCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) FCR_HD_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the Mux Segment.
  * @note   This function can be used only when the LCD is disabled.
  * @param  NewState: new state of the Mux Segment.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LCD_MuxSegmentCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CR_MUX_SEG_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the LCD pulses on duration.
  * @param  LCD_PulseOnDuration: specifies the LCD pulse on duration in terms of
  *         CK_PS (prescaled LCD clock period) pulses.
  *   This parameter can be one of the following values:
  *     @arg LCD_PulseOnDuration_0: 0 pulse
  *     @arg LCD_PulseOnDuration_1: Pulse ON duration = 1/CK_PS
  *     @arg LCD_PulseOnDuration_2: Pulse ON duration = 2/CK_PS
  *     @arg LCD_PulseOnDuration_3: Pulse ON duration = 3/CK_PS
  *     @arg LCD_PulseOnDuration_4: Pulse ON duration = 4/CK_PS
  *     @arg LCD_PulseOnDuration_5: Pulse ON duration = 5/CK_PS
  *     @arg LCD_PulseOnDuration_6: Pulse ON duration = 6/CK_PS
  *     @arg LCD_PulseOnDuration_7: Pulse ON duration = 7/CK_PS
  * @retval None
  */
void LCD_PulseOnDurationConfig(uint32_t LCD_PulseOnDuration)
{
  LCD->FCR &= (uint32_t)PON_MASK;
  LCD->FCR |= (uint32_t)(LCD_PulseOnDuration);
}

/**
  * @brief  Configures the LCD dead time.
  * @param  LCD_DeadTime: specifies the LCD dead time.
  *   This parameter can be one of the following values:
  *     @arg LCD_DeadTime_0: No dead Time
  *     @arg LCD_DeadTime_1: One Phase between different couple of Frame
  *     @arg LCD_DeadTime_2: Two Phase between different couple of Frame
  *     @arg LCD_DeadTime_3: Three Phase between different couple of Frame
  *     @arg LCD_DeadTime_4: Four Phase between different couple of Frame
  *     @arg LCD_DeadTime_5: Five Phase between different couple of Frame
  *     @arg LCD_DeadTime_6: Six Phase between different couple of Frame 
  *     @arg LCD_DeadTime_7: Seven Phase between different couple of Frame
  * @retval None
  */
void LCD_DeadTimeConfig(uint32_t LCD_DeadTime)
{
  LCD->FCR &= (uint32_t)DEAD_MASK;
  LCD->FCR |= (uint32_t)(LCD_DeadTime);
}

/**
  * @brief  Configures the LCD Blink mode and Blink frequency.
  * @param  LCD_BlinkMode: specifies the LCD blink mode.
  *   This parameter can be one of the following values:
  *     @arg LCD_BlinkMode_Off:           Blink disabled
  *     @arg LCD_BlinkMode_SEG0_COM0:     Blink enabled on SEG[0], COM[0] (1 pixel)
  *     @arg LCD_BlinkMode_SEG0_AllCOM:   Blink enabled on SEG[0], all COM (up to 8
  *                                       pixels according to the programmed duty)
  *     @arg LCD_BlinkMode_AllSEG_AllCOM: Blink enabled on all SEG and all COM 
  *                                       (all pixels)
  * @param  LCD_BlinkFrequency: specifies the LCD blink frequency.
  *   This parameter can be one of the following values:
  *     @arg LCD_BlinkFrequency_Div8:    The Blink frequency = fLcd/8
  *     @arg LCD_BlinkFrequency_Div16:   The Blink frequency = fLcd/16
  *     @arg LCD_BlinkFrequency_Div32:   The Blink frequency = fLcd/32
  *     @arg LCD_BlinkFrequency_Div64:   The Blink frequency = fLcd/64
  *     @arg LCD_BlinkFrequency_Div128:  The Blink frequency = fLcd/128
  *     @arg LCD_BlinkFrequency_Div256:  The Blink frequency = fLcd/256
  *     @arg LCD_BlinkFrequency_Div512:  The Blink frequency = fLcd/512
  *     @arg LCD_BlinkFrequency_Div1024: The Blink frequency = fLcd/1024
  * @retval None
  */
void LCD_BlinkConfig(uint32_t LCD_BlinkMode, uint32_t LCD_BlinkFrequency)
{
  LCD->FCR &= (uint32_t)BLINK_MASK;
  LCD->FCR |= (uint32_t)(LCD_BlinkMode | LCD_BlinkFrequency);
}

/**
  * @brief  Configures the LCD Contrast.
  * @param  LCD_Contrast: specifies the LCD Contrast.
  *   This parameter can be one of the following values:
  *     @arg LCD_Contrast_Level_0: Maximum Voltage = 2.60V
  *     @arg LCD_Contrast_Level_1: Maximum Voltage = 2.73V
  *     @arg LCD_Contrast_Level_2: Maximum Voltage = 2.86V
  *     @arg LCD_Contrast_Level_3: Maximum Voltage = 2.99V
  *     @arg LCD_Contrast_Level_4: Maximum Voltage = 3.12V
  *     @arg LCD_Contrast_Level_5: Maximum Voltage = 3.25V
  *     @arg LCD_Contrast_Level_6: Maximum Voltage = 3.38V
  *     @arg LCD_Contrast_Level_7: Maximum Voltage = 3.51V
  * @retval None
  */
void LCD_ContrastConfig(uint32_t LCD_Contrast)
{
  LCD->FCR &= (uint32_t)CONTRAST_MASK;
  LCD->FCR |= (uint32_t)(LCD_Contrast);
}

/**
  * @brief  Writes a word in the specific LCD RAM.
  * @param  LCD_RAMRegister: specifies the LCD Contrast.
  *   This parameter can be one of the following values:
  *     @arg LCD_RAMRegister_0: LCD RAM Register 0
  *     @arg LCD_RAMRegister_1: LCD RAM Register 1
  *     @arg LCD_RAMRegister_2: LCD RAM Register 2
  *     @arg LCD_RAMRegister_3: LCD RAM Register 3
  *     @arg LCD_RAMRegister_4: LCD RAM Register 4
  *     @arg LCD_RAMRegister_5: LCD RAM Register 5
  *     @arg LCD_RAMRegister_6: LCD RAM Register 6
  *     @arg LCD_RAMRegister_7: LCD RAM Register 7
  *     @arg LCD_RAMRegister_8: LCD RAM Register 8
  *     @arg LCD_RAMRegister_9: LCD RAM Register 9
  *     @arg LCD_RAMRegister_10: LCD RAM Register 10
  *     @arg LCD_RAMRegister_11: LCD RAM Register 11
  *     @arg LCD_RAMRegister_12: LCD RAM Register 12
  *     @arg LCD_RAMRegister_13: LCD RAM Register 13
  *     @arg LCD_RAMRegister_14: LCD RAM Register 14
  *     @arg LCD_RAMRegister_15: LCD RAM Register 15
  * @param  LCD_Data: specifies LCD Data Value to be written.
  * @retval None
  */
void LCD_Write(uint32_t LCD_RAMRegister, uint32_t LCD_Data)
{
  /* Copy data bytes to RAM register */
  LCD->RAM[LCD_RAMRegister] = (uint32_t)LCD_Data;
}

/**
  * @brief  Enables the Update Display Request.
  * @note   Each time software modifies the LCD_RAM it must set the UDR bit to
  *         transfer the updated data to the second level buffer.
  *         The UDR bit stays set until the end of the update and during this
  *         time the LCD_RAM is write protected.
  * @note   When the display is disabled, the update is performed for all
  *         LCD_DISPLAY locations.
  *         When the display is enabled, the update is performed only for locations
  *         for which commons are active (depending on DUTY). For example if
  *         DUTY = 1/2, only the LCD_DISPLAY of COM0 and COM1 will be updated.
  * @param  None
  * @retval None
  */
void LCD_UpdateDisplayRequest(void)
{
  *(__IO uint32_t *) SR_UDR_BB = (uint32_t)0x01;
}

/**
  * @brief  Enables or disables the specified LCD interrupts.
  * @param  LCD_IT: specifies the LCD interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg LCD_IT_SOF: Start of Frame Interrupt
  *     @arg LCD_IT_UDD: Update Display Done Interrupt
  * @param NewState: new state of the specified LCD interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LCD_ITConfig(uint32_t LCD_IT, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    LCD->FCR |= LCD_IT;
  }
  else
  {
    LCD->FCR &= (uint32_t)~LCD_IT;
  }
}

/**
  * @brief  Checks whether the specified LCD flag is set or not.
  * @param  LCD_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg LCD_FLAG_ENS: LCD Enabled flag. It indicates the LCD controller status.
  *      @note  The ENS bit is set immediately when the LCDEN bit in the LCD_CR
  *             goes from 0 to 1. On deactivation it reflects the real status of
  *             LCD so it becomes 0 at the end of the last displayed frame.
  *     @arg LCD_FLAG_SOF: Start of Frame flag. This flag is set by hardware at
  *       the beginning of a new frame, at the same time as the display data is
  *       updated.
  *     @arg LCD_FLAG_UDR: Update Display Request flag.
  *     @arg LCD_FLAG_UDD: Update Display Done flag.
  *     @arg LCD_FLAG_RDY: Step_up converter Ready flag. It indicates the status
  *                        of the step-up converter.
  *     @arg LCD_FLAG_FCRSF: LCD Frame Control Register Synchronization Flag.
  *       This flag is set by hardware each time the LCD_FCR register is updated
  *       in the LCDCLK domain.
  * @retval The new state of LCD_FLAG (SET or RESET).
  */
FlagStatus LCD_GetFlagStatus(uint32_t LCD_FLAG)
{
  FlagStatus bitstatus = RESET;
  
  if ((LCD->SR & LCD_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  Clears the LCD's pending flags.
  * @param  LCD_FLAG: specifies the flag to clear.
  *   This parameter can be any combination of the following values:
  *     @arg LCD_FLAG_SOF: Start of Frame Interrupt
  *     @arg LCD_FLAG_UDD: Update Display Done Interrupt
  * @retval None
  */
void LCD_ClearFlag(uint32_t LCD_FLAG)
{
  /* Clear the corresponding LCD flag */
  LCD->CLR = (uint32_t)LCD_FLAG;
}

/**
  * @brief  Checks whether the specified RTC interrupt has occurred or not.
  * @param  LCD_IT: specifies the LCD interrupts sources to check.
  *   This parameter can be one of the following values:
  *     @arg LCD_IT_SOF: Start of Frame Interrupt
  *     @arg LCD_IT_UDD: Update Display Done Interrupt.
  *     @note If the device is in STOP mode (PCLK not provided) UDD will not 
  *          generate an interrupt even if UDDIE = 1. 
  *          If the display is not enabled the UDD interrupt will never occur.
  * @retval The new state of the LCD_IT (SET or RESET).
  */
ITStatus LCD_GetITStatus(uint32_t LCD_IT)
{
  ITStatus bitstatus = RESET;
  
  if ((LCD->SR & LCD_IT) != (uint16_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  
  if (((LCD->FCR & LCD_IT) != (uint16_t)RESET) && (bitstatus != (uint32_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  Clears the LCD's interrupt pending bits.
  * @param  LCD_IT: specifies the interrupt pending bit to clear.
  *   This parameter can be any combination of the following values:
  *     @arg LCD_IT_SOF: Start of Frame Interrupt
  *     @arg LCD_IT_UDD: Update Display Done Interrupt
  * @retval None
  */
void LCD_ClearITPendingBit(uint32_t LCD_IT)
{
  /* Clear the corresponding LCD pending bit */
  LCD->CLR = (uint32_t)LCD_IT;
}
