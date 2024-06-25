#ifndef _RTC_H
#define _RTC_H

/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  RTC Init structures definition  
  */ 
typedef struct
{
  uint32_t RTC_HourFormat;   /*!< Specifies the RTC Hour Format.
                             This parameter can be a value of @ref RTC_Hour_Formats */
  
  uint32_t RTC_AsynchPrediv; /*!< Specifies the RTC Asynchronous Predivider value.
                             This parameter must be set to a value lower than 0x7F */
  
  uint32_t RTC_SynchPrediv;  /*!< Specifies the RTC Synchronous Predivider value.
                             This parameter must be set to a value lower than 0x7FFF */
}RTC_InitTypeDef;

/** 
  * @brief  RTC Time structure definition  
  */
typedef struct
{
  uint8_t RTC_Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */
  
  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_H12;      /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef; 

/** 
  * @brief  RTC Date structure definition  
  */
typedef struct
{
  uint8_t RTC_WeekDay; /*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */
  
  uint8_t RTC_Month;   /*!< Specifies the RTC Date Month (in BCD format).
                        This parameter can be a value of @ref RTC_Month_Date_Definitions */

  uint8_t RTC_Date;     /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */
  
  uint8_t RTC_Year;     /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
}RTC_DateTypeDef;

/** 
  * @brief  RTC Alarm structure definition  
  */
typedef struct
{
  RTC_TimeTypeDef RTC_AlarmTime;     /*!< Specifies the RTC Alarm Time members. */

  uint32_t RTC_AlarmMask;            /*!< Specifies the RTC Alarm Masks.
                                     This parameter can be a value of @ref RTC_AlarmMask_Definitions */

  uint32_t RTC_AlarmDateWeekDaySel;  /*!< Specifies the RTC Alarm is on Date or WeekDay.
                                     This parameter can be a value of @ref RTC_AlarmDateWeekDay_Definitions */
  
  uint8_t RTC_AlarmDateWeekDay;      /*!< Specifies the RTC Alarm Date/WeekDay.
                                     If the Alarm Date is selected, this parameter
                                     must be set to a value in the 1-31 range.
                                     If the Alarm WeekDay is selected, this 
                                     parameter can be a value of @ref RTC_WeekDay_Definitions */
}RTC_AlarmTypeDef;

/* Exported constants --------------------------------------------------------*/

#define RTC_HourFormat_24              ((uint32_t)0x00000000)
#define RTC_HourFormat_12              ((uint32_t)0x00000040)

#define RTC_H12_AM                     ((uint8_t)0x00)
#define RTC_H12_PM                     ((uint8_t)0x40)

/* Coded in BCD format */
#define RTC_Month_January              ((uint8_t)0x01)
#define RTC_Month_February             ((uint8_t)0x02)
#define RTC_Month_March                ((uint8_t)0x03)
#define RTC_Month_April                ((uint8_t)0x04)
#define RTC_Month_May                  ((uint8_t)0x05)
#define RTC_Month_June                 ((uint8_t)0x06)
#define RTC_Month_July                 ((uint8_t)0x07)
#define RTC_Month_August               ((uint8_t)0x08)
#define RTC_Month_September            ((uint8_t)0x09)
#define RTC_Month_October              ((uint8_t)0x10)
#define RTC_Month_November             ((uint8_t)0x11)
#define RTC_Month_December             ((uint8_t)0x12)
  
#define RTC_Weekday_Monday             ((uint8_t)0x01)
#define RTC_Weekday_Tuesday            ((uint8_t)0x02)
#define RTC_Weekday_Wednesday          ((uint8_t)0x03)
#define RTC_Weekday_Thursday           ((uint8_t)0x04)
#define RTC_Weekday_Friday             ((uint8_t)0x05)
#define RTC_Weekday_Saturday           ((uint8_t)0x06)
#define RTC_Weekday_Sunday             ((uint8_t)0x07)

#define RTC_AlarmDateWeekDaySel_Date      ((uint32_t)0x00000000)
#define RTC_AlarmDateWeekDaySel_WeekDay   ((uint32_t)0x40000000)

#define RTC_AlarmMask_None                ((uint32_t)0x00000000)
#define RTC_AlarmMask_DateWeekDay         ((uint32_t)0x80000000)
#define RTC_AlarmMask_Hours               ((uint32_t)0x00800000)
#define RTC_AlarmMask_Minutes             ((uint32_t)0x00008000)
#define RTC_AlarmMask_Seconds             ((uint32_t)0x00000080)
#define RTC_AlarmMask_All                 ((uint32_t)0x80808080)

#define RTC_Alarm_A                       ((uint32_t)0x00000100)
#define RTC_Alarm_B                       ((uint32_t)0x00000200)

#define RTC_AlarmSubSecondMask_All         ((uint32_t)0x00000000) /*!< All Alarm SS fields are masked. 
                                                                       There is no comparison on sub seconds 
                                                                       for Alarm */
#define RTC_AlarmSubSecondMask_SS14_1      ((uint32_t)0x01000000) /*!< SS[14:1] are don't care in Alarm 
                                                                       comparison. Only SS[0] is compared. */
#define RTC_AlarmSubSecondMask_SS14_2      ((uint32_t)0x02000000) /*!< SS[14:2] are don't care in Alarm 
                                                                       comparison. Only SS[1:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_3      ((uint32_t)0x03000000) /*!< SS[14:3] are don't care in Alarm 
                                                                       comparison. Only SS[2:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_4      ((uint32_t)0x04000000) /*!< SS[14:4] are don't care in Alarm 
                                                                       comparison. Only SS[3:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_5      ((uint32_t)0x05000000) /*!< SS[14:5] are don't care in Alarm 
                                                                       comparison. Only SS[4:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_6      ((uint32_t)0x06000000) /*!< SS[14:6] are don't care in Alarm 
                                                                       comparison. Only SS[5:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_7      ((uint32_t)0x07000000) /*!< SS[14:7] are don't care in Alarm 
                                                                       comparison. Only SS[6:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_8      ((uint32_t)0x08000000) /*!< SS[14:8] are don't care in Alarm 
                                                                       comparison. Only SS[7:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_9      ((uint32_t)0x09000000) /*!< SS[14:9] are don't care in Alarm 
                                                                       comparison. Only SS[8:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_10     ((uint32_t)0x0A000000) /*!< SS[14:10] are don't care in Alarm 
                                                                       comparison. Only SS[9:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_11     ((uint32_t)0x0B000000) /*!< SS[14:11] are don't care in Alarm 
                                                                       comparison. Only SS[10:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_12     ((uint32_t)0x0C000000) /*!< SS[14:12] are don't care in Alarm 
                                                                       comparison.Only SS[11:0] are compared */
#define RTC_AlarmSubSecondMask_SS14_13     ((uint32_t)0x0D000000) /*!< SS[14:13] are don't care in Alarm 
                                                                       comparison. Only SS[12:0] are compared */
#define RTC_AlarmSubSecondMask_SS14        ((uint32_t)0x0E000000) /*!< SS[14] is don't care in Alarm 
                                                                       comparison.Only SS[13:0] are compared */
#define RTC_AlarmSubSecondMask_None        ((uint32_t)0x0F000000) /*!< SS[14:0] are compared and must match 
                                                                       to activate alarm. */
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)

#define RTC_TimeStampEdge_Rising          ((uint32_t)0x00000000)
#define RTC_TimeStampEdge_Falling         ((uint32_t)0x00000008)

#define RTC_Output_Disable             ((uint32_t)0x00000000)
#define RTC_Output_AlarmA              ((uint32_t)0x00200000)
#define RTC_Output_AlarmB              ((uint32_t)0x00400000)
#define RTC_Output_WakeUp              ((uint32_t)0x00600000)

#define RTC_OutputPolarity_High           ((uint32_t)0x00000000)
#define RTC_OutputPolarity_Low            ((uint32_t)0x00100000)

#define RTC_CalibSign_Positive            ((uint32_t)0x00000000) 
#define RTC_CalibSign_Negative            ((uint32_t)0x00000080)

#define RTC_CalibOutput_512Hz            ((uint32_t)0x00000000) 
#define RTC_CalibOutput_1Hz              ((uint32_t)0x00080000)

#define RTC_SmoothCalibPeriod_32sec   ((uint32_t)0x00000000) /*!<  if RTCCLK = 32768 Hz, Smooth calibation
                                                             period is 32s,  else 2exp20 RTCCLK seconds */
#define RTC_SmoothCalibPeriod_16sec   ((uint32_t)0x00002000) /*!<  if RTCCLK = 32768 Hz, Smooth calibration 
                                                             period is 16s, else 2exp19 RTCCLK seconds */
#define RTC_SmoothCalibPeriod_8sec    ((uint32_t)0x00004000) /*!<  if RTCCLK = 32768 Hz, Smooth calibation 
                                                             period is 8s, else 2exp18 RTCCLK seconds */

#define RTC_SmoothCalibPlusPulses_Set    ((uint32_t)0x00008000) /*!<  The number of RTCCLK pulses added  
                                                                during a X -second window = Y - CALM[8:0]. 
                                                                 with Y = 512, 256, 128 when X = 32, 16, 8 */
#define RTC_SmoothCalibPlusPulses_Reset  ((uint32_t)0x00000000) /*!<  The number of RTCCLK pulses subbstited
                                                                 during a 32-second window =   CALM[8:0]. */

#define RTC_DayLightSaving_SUB1H   ((uint32_t)0x00020000)
#define RTC_DayLightSaving_ADD1H   ((uint32_t)0x00010000)

#define RTC_StoreOperation_Reset        ((uint32_t)0x00000000)
#define RTC_StoreOperation_Set          ((uint32_t)0x00040000)

#define RTC_TamperTrigger_RisingEdge            ((uint32_t)0x00000000)
#define RTC_TamperTrigger_FallingEdge           ((uint32_t)0x00000001)
#define RTC_TamperTrigger_LowLevel              ((uint32_t)0x00000000)
#define RTC_TamperTrigger_HighLevel             ((uint32_t)0x00000001)

#define RTC_TamperFilter_Disable   ((uint32_t)0x00000000) /*!< Tamper filter is disabled */

#define RTC_TamperFilter_2Sample   ((uint32_t)0x00000800) /*!< Tamper is activated after 2 
                                                          consecutive samples at the active level */
#define RTC_TamperFilter_4Sample   ((uint32_t)0x00001000) /*!< Tamper is activated after 4 
                                                          consecutive samples at the active level */
#define RTC_TamperFilter_8Sample   ((uint32_t)0x00001800) /*!< Tamper is activated after 8 
                                                          consecutive samples at the active level. */

#define RTC_TamperSamplingFreq_RTCCLK_Div32768  ((uint32_t)0x00000000) /*!< Each of the tamper inputs are sampled
                                                                           with a frequency =  RTCCLK / 32768 */
#define RTC_TamperSamplingFreq_RTCCLK_Div16384  ((uint32_t)0x000000100) /*!< Each of the tamper inputs are sampled
                                                                            with a frequency =  RTCCLK / 16384 */
#define RTC_TamperSamplingFreq_RTCCLK_Div8192   ((uint32_t)0x00000200) /*!< Each of the tamper inputs are sampled
                                                                           with a frequency =  RTCCLK / 8192  */
#define RTC_TamperSamplingFreq_RTCCLK_Div4096   ((uint32_t)0x00000300) /*!< Each of the tamper inputs are sampled
                                                                           with a frequency =  RTCCLK / 4096  */
#define RTC_TamperSamplingFreq_RTCCLK_Div2048   ((uint32_t)0x00000400) /*!< Each of the tamper inputs are sampled
                                                                           with a frequency =  RTCCLK / 2048  */
#define RTC_TamperSamplingFreq_RTCCLK_Div1024   ((uint32_t)0x00000500) /*!< Each of the tamper inputs are sampled
                                                                           with a frequency =  RTCCLK / 1024  */
#define RTC_TamperSamplingFreq_RTCCLK_Div512    ((uint32_t)0x00000600) /*!< Each of the tamper inputs are sampled
                                                                           with a frequency =  RTCCLK / 512   */
#define RTC_TamperSamplingFreq_RTCCLK_Div256    ((uint32_t)0x00000700) /*!< Each of the tamper inputs are sampled
                                                                           with a frequency =  RTCCLK / 256   */

#define RTC_TamperPrechargeDuration_1RTCCLK ((uint32_t)0x00000000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 1 RTCCLK cycle */
#define RTC_TamperPrechargeDuration_2RTCCLK ((uint32_t)0x00002000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 2 RTCCLK cycles */
#define RTC_TamperPrechargeDuration_4RTCCLK ((uint32_t)0x00004000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 4 RTCCLK cycles */
#define RTC_TamperPrechargeDuration_8RTCCLK ((uint32_t)0x00006000)  /*!< Tamper pins are pre-charged before 
                                                                         sampling during 8 RTCCLK cycles */

#define RTC_Tamper_1 RTC_TAFCR_TAMP1E
#define RTC_Tamper_2 RTC_TAFCR_TAMP2E

#define RTC_TamperPin_Default       ((uint32_t)0x00000000)
#define RTC_TamperPin_Pos1          ((uint32_t)0x00010000)

/* Legacy Defines */
#define RTC_TamperPin_PC13           RTC_TamperPin_Default
#define RTC_TamperPin_PI8            RTC_TamperPin_Pos1

#define RTC_TimeStampPin_PC13              ((uint32_t)0x00000000)
#define RTC_TimeStampPin_PI8               ((uint32_t)0x00020000)

#define RTC_OutputType_OpenDrain           ((uint32_t)0x00000000)
#define RTC_OutputType_PushPull            ((uint32_t)0x00040000)

#define RTC_ShiftAdd1S_Reset      ((uint32_t)0x00000000)
#define RTC_ShiftAdd1S_Set        ((uint32_t)0x80000000)

#define RTC_BKP_DR0                       ((uint32_t)0x00000000)
#define RTC_BKP_DR1                       ((uint32_t)0x00000001)
#define RTC_BKP_DR2                       ((uint32_t)0x00000002)
#define RTC_BKP_DR3                       ((uint32_t)0x00000003)
#define RTC_BKP_DR4                       ((uint32_t)0x00000004)
#define RTC_BKP_DR5                       ((uint32_t)0x00000005)
#define RTC_BKP_DR6                       ((uint32_t)0x00000006)
#define RTC_BKP_DR7                       ((uint32_t)0x00000007)
#define RTC_BKP_DR8                       ((uint32_t)0x00000008)
#define RTC_BKP_DR9                       ((uint32_t)0x00000009)
#define RTC_BKP_DR10                      ((uint32_t)0x0000000A)
#define RTC_BKP_DR11                      ((uint32_t)0x0000000B)
#define RTC_BKP_DR12                      ((uint32_t)0x0000000C)
#define RTC_BKP_DR13                      ((uint32_t)0x0000000D)
#define RTC_BKP_DR14                      ((uint32_t)0x0000000E)
#define RTC_BKP_DR15                      ((uint32_t)0x0000000F)
#define RTC_BKP_DR16                      ((uint32_t)0x00000010)
#define RTC_BKP_DR17                      ((uint32_t)0x00000011)
#define RTC_BKP_DR18                      ((uint32_t)0x00000012)
#define RTC_BKP_DR19                      ((uint32_t)0x00000013)

#define RTC_Format_BIN                    ((uint32_t)0x000000000)
#define RTC_Format_BCD                    ((uint32_t)0x000000001)

#define RTC_FLAG_RECALPF                  ((uint32_t)0x00010000)
#define RTC_FLAG_TAMP1F                   ((uint32_t)0x00002000)
#define RTC_FLAG_TAMP2F                   ((uint32_t)0x00004000) 
#define RTC_FLAG_TSOVF                    ((uint32_t)0x00001000)
#define RTC_FLAG_TSF                      ((uint32_t)0x00000800)
#define RTC_FLAG_WUTF                     ((uint32_t)0x00000400)
#define RTC_FLAG_ALRBF                    ((uint32_t)0x00000200)
#define RTC_FLAG_ALRAF                    ((uint32_t)0x00000100)
#define RTC_FLAG_INITF                    ((uint32_t)0x00000040)
#define RTC_FLAG_RSF                      ((uint32_t)0x00000020)
#define RTC_FLAG_INITS                    ((uint32_t)0x00000010)
#define RTC_FLAG_SHPF                     ((uint32_t)0x00000008)
#define RTC_FLAG_WUTWF                    ((uint32_t)0x00000004)
#define RTC_FLAG_ALRBWF                   ((uint32_t)0x00000002)
#define RTC_FLAG_ALRAWF                   ((uint32_t)0x00000001)

#define RTC_IT_TS                         ((uint32_t)0x00008000)
#define RTC_IT_WUT                        ((uint32_t)0x00004000)
#define RTC_IT_ALRB                       ((uint32_t)0x00002000)
#define RTC_IT_ALRA                       ((uint32_t)0x00001000)
#define RTC_IT_TAMP                       ((uint32_t)0x00000004) /* Used only to Enable the Tamper Interrupt */
#define RTC_IT_TAMP1                      ((uint32_t)0x00020000)
#define RTC_IT_TAMP2                      ((uint32_t)0x00040000)

#define RTC_DigitalCalibConfig  RTC_CoarseCalibConfig
#define RTC_DigitalCalibCmd     RTC_CoarseCalibCmd

/* Exported functions --------------------------------------------------------*/ 

/*  Function used to set the RTC configuration to the default reset state *****/
ErrorStatus RTC_DeInit(void);

/* Initialization and Configuration functions *********************************/
ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);
void RTC_WriteProtectionCmd(FunctionalState NewState);
ErrorStatus RTC_EnterInitMode(void);
void RTC_ExitInitMode(void);
ErrorStatus RTC_WaitForSynchro(void);
ErrorStatus RTC_RefClockCmd(FunctionalState NewState);
void RTC_BypassShadowCmd(FunctionalState NewState);

/* Time and Date configuration functions **************************************/
ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct);
void RTC_GetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
uint32_t RTC_GetSubSecond(void);
ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);
void RTC_DateStructInit(RTC_DateTypeDef* RTC_DateStruct);
void RTC_GetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);

/* Alarms (Alarm A and Alarm B) configuration functions  **********************/
void RTC_SetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct);
void RTC_GetAlarm(uint32_t RTC_Format, uint32_t RTC_Alarm, RTC_AlarmTypeDef* RTC_AlarmStruct);
ErrorStatus RTC_AlarmCmd(uint32_t RTC_Alarm, FunctionalState NewState);
void RTC_AlarmSubSecondConfig(uint32_t RTC_Alarm, uint32_t RTC_AlarmSubSecondValue, uint32_t RTC_AlarmSubSecondMask);
uint32_t RTC_GetAlarmSubSecond(uint32_t RTC_Alarm);

/* WakeUp Timer configuration functions ***************************************/
void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock);
void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter);
uint32_t RTC_GetWakeUpCounter(void);
ErrorStatus RTC_WakeUpCmd(FunctionalState NewState);

/* Daylight Saving configuration functions ************************************/
void RTC_DayLightSavingConfig(uint32_t RTC_DayLightSaving, uint32_t RTC_StoreOperation);
uint32_t RTC_GetStoreOperation(void);

/* Output pin Configuration function ******************************************/
void RTC_OutputConfig(uint32_t RTC_Output, uint32_t RTC_OutputPolarity);

/* Digital Calibration configuration functions *********************************/
ErrorStatus RTC_CoarseCalibConfig(uint32_t RTC_CalibSign, uint32_t Value);
ErrorStatus RTC_CoarseCalibCmd(FunctionalState NewState);
void RTC_CalibOutputCmd(FunctionalState NewState);
void RTC_CalibOutputConfig(uint32_t RTC_CalibOutput);
ErrorStatus RTC_SmoothCalibConfig(uint32_t RTC_SmoothCalibPeriod, 
                                  uint32_t RTC_SmoothCalibPlusPulses,
                                  uint32_t RTC_SmouthCalibMinusPulsesValue);

/* TimeStamp configuration functions ******************************************/
void RTC_TimeStampCmd(uint32_t RTC_TimeStampEdge, FunctionalState NewState);
void RTC_GetTimeStamp(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_StampTimeStruct,
                                      RTC_DateTypeDef* RTC_StampDateStruct);
uint32_t RTC_GetTimeStampSubSecond(void);

/* Tampers configuration functions ********************************************/
void RTC_TamperTriggerConfig(uint32_t RTC_Tamper, uint32_t RTC_TamperTrigger);
void RTC_TamperCmd(uint32_t RTC_Tamper, FunctionalState NewState);
void RTC_TamperFilterConfig(uint32_t RTC_TamperFilter);
void RTC_TamperSamplingFreqConfig(uint32_t RTC_TamperSamplingFreq);
void RTC_TamperPinsPrechargeDuration(uint32_t RTC_TamperPrechargeDuration);
void RTC_TimeStampOnTamperDetectionCmd(FunctionalState NewState);
void RTC_TamperPullUpCmd(FunctionalState NewState);

/* Backup Data Registers configuration functions ******************************/
void RTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data);
uint32_t RTC_ReadBackupRegister(uint32_t RTC_BKP_DR);

/* RTC Tamper and TimeStamp Pins Selection and Output Type Config configuration
   functions ******************************************************************/
void RTC_TamperPinSelection(uint32_t RTC_TamperPin);
void RTC_TimeStampPinSelection(uint32_t RTC_TimeStampPin);
void RTC_OutputTypeConfig(uint32_t RTC_OutputType);

/* RTC_Shift_control_synchonisation_functions *********************************/
ErrorStatus RTC_SynchroShiftConfig(uint32_t RTC_ShiftAdd1S, uint32_t RTC_ShiftSubFS);

/* Interrupts and flags management functions **********************************/
void RTC_ITConfig(uint32_t RTC_IT, FunctionalState NewState);
FlagStatus RTC_GetFlagStatus(uint32_t RTC_FLAG);
void RTC_ClearFlag(uint32_t RTC_FLAG);
ITStatus RTC_GetITStatus(uint32_t RTC_IT);
void RTC_ClearITPendingBit(uint32_t RTC_IT);

#endif /* _RTC_H */
