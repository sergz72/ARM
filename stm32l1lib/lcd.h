#ifndef _LCD_H
#define _LCD_H

/** 
  * @brief  LCD Init structure definition  
  */

typedef struct
{
  uint32_t LCD_Prescaler;     /*!< Configures the LCD Prescaler. 
                                   This parameter can be one value of @ref LCD_Prescaler */
  uint32_t LCD_Divider;       /*!< Configures the LCD Divider.
                                  This parameter can be one value of @ref LCD_Divider */
  uint32_t LCD_Duty;          /*!< Configures the LCD Duty.
                                  This parameter can be one value of @ref LCD_Duty */
  uint32_t LCD_Bias;          /*!< Configures the LCD Bias.
                                  This parameter can be one value of @ref LCD_Bias */ 
  uint32_t LCD_VoltageSource; /*!< Selects the LCD Voltage source.
                                  This parameter can be one value of @ref LCD_Voltage_Source */
}LCD_InitTypeDef;

#define LCD_Prescaler_1        ((uint32_t)0x00000000)  /*!< CLKPS = LCDCLK        */
#define LCD_Prescaler_2        ((uint32_t)0x00400000)  /*!< CLKPS = LCDCLK/2      */
#define LCD_Prescaler_4        ((uint32_t)0x00800000)  /*!< CLKPS = LCDCLK/4      */
#define LCD_Prescaler_8        ((uint32_t)0x00C00000)  /*!< CLKPS = LCDCLK/8      */
#define LCD_Prescaler_16       ((uint32_t)0x01000000)  /*!< CLKPS = LCDCLK/16     */
#define LCD_Prescaler_32       ((uint32_t)0x01400000)  /*!< CLKPS = LCDCLK/32     */
#define LCD_Prescaler_64       ((uint32_t)0x01800000)  /*!< CLKPS = LCDCLK/64     */
#define LCD_Prescaler_128      ((uint32_t)0x01C00000)  /*!< CLKPS = LCDCLK/128    */
#define LCD_Prescaler_256      ((uint32_t)0x02000000)  /*!< CLKPS = LCDCLK/256    */
#define LCD_Prescaler_512      ((uint32_t)0x02400000)  /*!< CLKPS = LCDCLK/512    */
#define LCD_Prescaler_1024     ((uint32_t)0x02800000)  /*!< CLKPS = LCDCLK/1024   */
#define LCD_Prescaler_2048     ((uint32_t)0x02C00000)  /*!< CLKPS = LCDCLK/2048   */
#define LCD_Prescaler_4096     ((uint32_t)0x03000000)  /*!< CLKPS = LCDCLK/4096   */
#define LCD_Prescaler_8192     ((uint32_t)0x03400000)  /*!< CLKPS = LCDCLK/8192   */
#define LCD_Prescaler_16384    ((uint32_t)0x03800000)  /*!< CLKPS = LCDCLK/16384  */
#define LCD_Prescaler_32768    ((uint32_t)0x03C00000)  /*!< CLKPS = LCDCLK/32768  */

#define LCD_Divider_16    ((uint32_t)0x00000000)  /*!< LCD frequency = CLKPS/16 */
#define LCD_Divider_17    ((uint32_t)0x00040000)  /*!< LCD frequency = CLKPS/17 */
#define LCD_Divider_18    ((uint32_t)0x00080000)  /*!< LCD frequency = CLKPS/18 */
#define LCD_Divider_19    ((uint32_t)0x000C0000)  /*!< LCD frequency = CLKPS/19 */
#define LCD_Divider_20    ((uint32_t)0x00100000)  /*!< LCD frequency = CLKPS/20 */
#define LCD_Divider_21    ((uint32_t)0x00140000)  /*!< LCD frequency = CLKPS/21 */
#define LCD_Divider_22    ((uint32_t)0x00180000)  /*!< LCD frequency = CLKPS/22 */
#define LCD_Divider_23    ((uint32_t)0x001C0000)  /*!< LCD frequency = CLKPS/23 */
#define LCD_Divider_24    ((uint32_t)0x00200000)  /*!< LCD frequency = CLKPS/24 */
#define LCD_Divider_25    ((uint32_t)0x00240000)  /*!< LCD frequency = CLKPS/25 */
#define LCD_Divider_26    ((uint32_t)0x00280000)  /*!< LCD frequency = CLKPS/26 */
#define LCD_Divider_27    ((uint32_t)0x002C0000)  /*!< LCD frequency = CLKPS/27 */
#define LCD_Divider_28    ((uint32_t)0x00300000)  /*!< LCD frequency = CLKPS/28 */
#define LCD_Divider_29    ((uint32_t)0x00340000)  /*!< LCD frequency = CLKPS/29 */
#define LCD_Divider_30    ((uint32_t)0x00380000)  /*!< LCD frequency = CLKPS/30 */
#define LCD_Divider_31    ((uint32_t)0x003C0000)  /*!< LCD frequency = CLKPS/31 */

#define LCD_Duty_Static                 ((uint32_t)0x00000000) /*!< Static duty */
#define LCD_Duty_1_2                    ((uint32_t)0x00000004) /*!< 1/2 duty    */
#define LCD_Duty_1_3                    ((uint32_t)0x00000008) /*!< 1/3 duty    */
#define LCD_Duty_1_4                    ((uint32_t)0x0000000C) /*!< 1/4 duty    */
#define LCD_Duty_1_8                    ((uint32_t)0x00000010) /*!< 1/4 duty    */
  
#define LCD_Bias_1_4                    ((uint32_t)0x00000000)  /*!< 1/4 Bias */
#define LCD_Bias_1_2                    LCD_CR_BIAS_0           /*!< 1/2 Bias */
#define LCD_Bias_1_3                    LCD_CR_BIAS_1           /*!< 1/3 Bias */
  
#define LCD_VoltageSource_Internal      ((uint32_t)0x00000000)  /*!< Internal voltage source for the LCD */
#define LCD_VoltageSource_External      LCD_CR_VSEL             /*!< External voltage source for the LCD */

#define LCD_IT_SOF                      LCD_FCR_SOFIE
#define LCD_IT_UDD                      LCD_FCR_UDDIE

#define LCD_PulseOnDuration_0           ((uint32_t)0x00000000) /*!< Pulse ON duration = 0 pulse   */
#define LCD_PulseOnDuration_1           ((uint32_t)0x00000010) /*!< Pulse ON duration = 1/CK_PS  */
#define LCD_PulseOnDuration_2           ((uint32_t)0x00000020) /*!< Pulse ON duration = 2/CK_PS  */
#define LCD_PulseOnDuration_3           ((uint32_t)0x00000030) /*!< Pulse ON duration = 3/CK_PS  */
#define LCD_PulseOnDuration_4           ((uint32_t)0x00000040) /*!< Pulse ON duration = 4/CK_PS  */
#define LCD_PulseOnDuration_5           ((uint32_t)0x00000050) /*!< Pulse ON duration = 5/CK_PS  */
#define LCD_PulseOnDuration_6           ((uint32_t)0x00000060) /*!< Pulse ON duration = 6/CK_PS  */
#define LCD_PulseOnDuration_7           ((uint32_t)0x00000070) /*!< Pulse ON duration = 7/CK_PS  */

#define LCD_DeadTime_0                  ((uint32_t)0x00000000) /*!< No dead Time  */
#define LCD_DeadTime_1                  ((uint32_t)0x00000080) /*!< One Phase between different couple of Frame   */
#define LCD_DeadTime_2                  ((uint32_t)0x00000100) /*!< Two Phase between different couple of Frame   */
#define LCD_DeadTime_3                  ((uint32_t)0x00000180) /*!< Three Phase between different couple of Frame */
#define LCD_DeadTime_4                  ((uint32_t)0x00000200) /*!< Four Phase between different couple of Frame  */
#define LCD_DeadTime_5                  ((uint32_t)0x00000280) /*!< Five Phase between different couple of Frame  */
#define LCD_DeadTime_6                  ((uint32_t)0x00000300) /*!< Six Phase between different couple of Frame   */
#define LCD_DeadTime_7                  ((uint32_t)0x00000380) /*!< Seven Phase between different couple of Frame */

#define LCD_BlinkMode_Off               ((uint32_t)0x00000000) /*!< Blink disabled            */
#define LCD_BlinkMode_SEG0_COM0         ((uint32_t)0x00010000) /*!< Blink enabled on SEG[0], COM[0] (1 pixel)   */
#define LCD_BlinkMode_SEG0_AllCOM       ((uint32_t)0x00020000) /*!< Blink enabled on SEG[0], all COM (up to 
                                                                    8 pixels according to the programmed duty)  */
#define LCD_BlinkMode_AllSEG_AllCOM     ((uint32_t)0x00030000) /*!< Blink enabled on all SEG and all COM (all pixels)  */

#define LCD_BlinkFrequency_Div8         ((uint32_t)0x00000000) /*!< The Blink frequency = fLCD/8    */
#define LCD_BlinkFrequency_Div16        ((uint32_t)0x00002000) /*!< The Blink frequency = fLCD/16   */
#define LCD_BlinkFrequency_Div32        ((uint32_t)0x00004000) /*!< The Blink frequency = fLCD/32   */
#define LCD_BlinkFrequency_Div64        ((uint32_t)0x00006000) /*!< The Blink frequency = fLCD/64   */
#define LCD_BlinkFrequency_Div128       ((uint32_t)0x00008000) /*!< The Blink frequency = fLCD/128  */
#define LCD_BlinkFrequency_Div256       ((uint32_t)0x0000A000) /*!< The Blink frequency = fLCD/256  */
#define LCD_BlinkFrequency_Div512       ((uint32_t)0x0000C000) /*!< The Blink frequency = fLCD/512  */
#define LCD_BlinkFrequency_Div1024      ((uint32_t)0x0000E000) /*!< The Blink frequency = fLCD/1024 */

#define LCD_Contrast_Level_0               ((uint32_t)0x00000000) /*!< Maximum Voltage = 2.60V    */
#define LCD_Contrast_Level_1               ((uint32_t)0x00000400) /*!< Maximum Voltage = 2.73V    */
#define LCD_Contrast_Level_2               ((uint32_t)0x00000800) /*!< Maximum Voltage = 2.86V    */
#define LCD_Contrast_Level_3               ((uint32_t)0x00000C00) /*!< Maximum Voltage = 2.99V    */
#define LCD_Contrast_Level_4               ((uint32_t)0x00001000) /*!< Maximum Voltage = 3.12V    */
#define LCD_Contrast_Level_5               ((uint32_t)0x00001400) /*!< Maximum Voltage = 3.25V    */
#define LCD_Contrast_Level_6               ((uint32_t)0x00001800) /*!< Maximum Voltage = 3.38V    */
#define LCD_Contrast_Level_7               ((uint32_t)0x00001C00) /*!< Maximum Voltage = 3.51V    */

#define LCD_FLAG_ENS                    LCD_SR_ENS
#define LCD_FLAG_SOF                    LCD_SR_SOF
#define LCD_FLAG_UDR                    LCD_SR_UDR
#define LCD_FLAG_UDD                    LCD_SR_UDD
#define LCD_FLAG_RDY                    LCD_SR_RDY
#define LCD_FLAG_FCRSF                  LCD_SR_FCRSR

#define LCD_RAMRegister_0               ((uint32_t)0x00000000) /*!< LCD RAM Register 0  */
#define LCD_RAMRegister_1               ((uint32_t)0x00000001) /*!< LCD RAM Register 1  */
#define LCD_RAMRegister_2               ((uint32_t)0x00000002) /*!< LCD RAM Register 2  */
#define LCD_RAMRegister_3               ((uint32_t)0x00000003) /*!< LCD RAM Register 3  */
#define LCD_RAMRegister_4               ((uint32_t)0x00000004) /*!< LCD RAM Register 4  */
#define LCD_RAMRegister_5               ((uint32_t)0x00000005) /*!< LCD RAM Register 5  */
#define LCD_RAMRegister_6               ((uint32_t)0x00000006) /*!< LCD RAM Register 6  */
#define LCD_RAMRegister_7               ((uint32_t)0x00000007) /*!< LCD RAM Register 7  */
#define LCD_RAMRegister_8               ((uint32_t)0x00000008) /*!< LCD RAM Register 8  */
#define LCD_RAMRegister_9               ((uint32_t)0x00000009) /*!< LCD RAM Register 9  */
#define LCD_RAMRegister_10              ((uint32_t)0x0000000A) /*!< LCD RAM Register 10 */
#define LCD_RAMRegister_11              ((uint32_t)0x0000000B) /*!< LCD RAM Register 11 */
#define LCD_RAMRegister_12              ((uint32_t)0x0000000C) /*!< LCD RAM Register 12 */
#define LCD_RAMRegister_13              ((uint32_t)0x0000000D) /*!< LCD RAM Register 13 */
#define LCD_RAMRegister_14              ((uint32_t)0x0000000E) /*!< LCD RAM Register 14 */
#define LCD_RAMRegister_15              ((uint32_t)0x0000000F) /*!< LCD RAM Register 15 */
/* Exported functions ------------------------------------------------------- */

/* Initialization and Configuration functions *********************************/
void LCD_Init(LCD_InitTypeDef* LCD_InitStruct);
void LCD_StructInit(LCD_InitTypeDef* LCD_InitStruct);
void LCD_Cmd(FunctionalState NewState);
void LCD_WaitForSynchro(void);
void LCD_HighDriveCmd(FunctionalState NewState);
void LCD_MuxSegmentCmd(FunctionalState NewState);
void LCD_PulseOnDurationConfig(uint32_t LCD_PulseOnDuration);
void LCD_DeadTimeConfig(uint32_t LCD_DeadTime);
void LCD_BlinkConfig(uint32_t LCD_BlinkMode, uint32_t LCD_BlinkFrequency);
void LCD_ContrastConfig(uint32_t LCD_Contrast);

/* LCD RAM memory write functions *********************************************/
void LCD_Write(uint32_t LCD_RAMRegister, uint32_t LCD_Data);
void LCD_UpdateDisplayRequest(void);

/* Interrupts and flags management functions **********************************/
void LCD_ITConfig(uint32_t LCD_IT, FunctionalState NewState);
FlagStatus LCD_GetFlagStatus(uint32_t LCD_FLAG);
void LCD_ClearFlag(uint32_t LCD_FLAG);
ITStatus LCD_GetITStatus(uint32_t LCD_IT);
void LCD_ClearITPendingBit(uint32_t LCD_IT);

#endif /* _LCD_H */
