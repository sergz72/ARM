#ifndef _SDRAM_H
#define _SDRAM_H

#include "fmc.h"
#include "dma.h"

/** 
  * @brief  HAL SDRAM State structure definition  
  */ 
typedef enum
{
  SDRAM_STATE_RESET             = 0x00U,  /*!< SDRAM not yet initialized or disabled */
  SDRAM_STATE_READY             = 0x01U,  /*!< SDRAM initialized and ready for use   */
  SDRAM_STATE_BUSY              = 0x02U,  /*!< SDRAM internal process is ongoing     */
  SDRAM_STATE_ERROR             = 0x03U,  /*!< SDRAM error state                     */
  SDRAM_STATE_WRITE_PROTECTED   = 0x04U,  /*!< SDRAM device write protected          */
  SDRAM_STATE_PRECHARGED        = 0x05U   /*!< SDRAM device precharged               */
} SDRAM_StateTypeDef;

/** 
  * @brief  SDRAM handle Structure definition  
  */ 
typedef struct
{
  FMC_SDRAM_TypeDef             *Instance;  /*!< Register base address                 */
  
  FMC_SDRAM_InitTypeDef         Init;       /*!< SDRAM device configuration parameters */
  
  __IO SDRAM_StateTypeDef       State;      /*!< SDRAM access state                    */
  
  DMA_HandleTypeDef             *hdma;      /*!< Pointer DMA handler                   */
  
} SDRAM_HandleTypeDef;

/** @brief Reset SDRAM handle state
  * @param  __HANDLE__: specifies the SDRAM handle.
  * @retval None
  */
#define _SDRAM_RESET_HANDLE_STATE(__HANDLE__) ((__HANDLE__)->State = SDRAM_STATE_RESET)

#define SDRAM_TIMEOUT                    0xFFFFU

/**
  * @brief  FMC SDRAM Mode definition register defines
  */
#define SDRAM_MODEREG_BURST_LENGTH_1             ((unsigned short)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((unsigned short)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((unsigned short)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((unsigned short)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((unsigned short)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((unsigned short)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((unsigned short)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((unsigned short)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((unsigned short)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((unsigned short)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((unsigned short)0x0200)

/** @addtogroup SDRAM_Exported_Functions_Group1 
  * @{
  */

/* Initialization/de-initialization functions *********************************/
unsigned int SDRAM_Init(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_TimingTypeDef *Timing);
void SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram);
unsigned int SDRAM_SendCommand(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command, unsigned int Timeout);
unsigned int SDRAM_ProgramRefreshRate(SDRAM_HandleTypeDef *hsdram, unsigned int RefreshRate);
int SDRAM_Test(void);

#endif /* _SDRAM_H */
