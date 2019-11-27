#ifndef _DMA_H
#define _DMA_H

#include <board.h>

#define DMA_FIFOMODE_DISABLE                     0x00000000U       /*!< FIFO mode disable */
#define DMA_FIFOMODE_ENABLE                      DMA_SxFCR_DMDIS   /*!< FIFO mode enable  */

#define DMA_MBURST_SINGLE                        0x00000000U
#define DMA_MBURST_INC4                          DMA_SxCR_MBURST_0
#define DMA_MBURST_INC8                          DMA_SxCR_MBURST_1
#define DMA_MBURST_INC16                         DMA_SxCR_MBURST

#define DMA_MDATAALIGN_BYTE                      0x00000000U        /*!< Memory data alignment: Byte     */
#define DMA_MDATAALIGN_HALFWORD                  DMA_SxCR_MSIZE_0   /*!< Memory data alignment: HalfWord */
#define DMA_MDATAALIGN_WORD                      DMA_SxCR_MSIZE_1   /*!< Memory data alignment: Word     */

#define DMA_FIFO_THRESHOLD_1QUARTERFULL          0x00000000U       /*!< FIFO threshold 1 quart full configuration  */
#define DMA_FIFO_THRESHOLD_HALFFULL              DMA_SxFCR_FTH_0   /*!< FIFO threshold half full configuration     */
#define DMA_FIFO_THRESHOLD_3QUARTERSFULL         DMA_SxFCR_FTH_1   /*!< FIFO threshold 3 quarts full configuration */
#define DMA_FIFO_THRESHOLD_FULL                  DMA_SxFCR_FTH     /*!< FIFO threshold full configuration          */

#define DMA_PERIPH_TO_MEMORY                     0x00000000U      /*!< Peripheral to memory direction */
#define DMA_MEMORY_TO_PERIPH                     DMA_SxCR_DIR_0   /*!< Memory to peripheral direction */
#define DMA_MEMORY_TO_MEMORY                     DMA_SxCR_DIR_1   /*!< Memory to memory direction     */

#define DMA_IT_TC                                DMA_SxCR_TCIE
#define DMA_IT_HT                                DMA_SxCR_HTIE
#define DMA_IT_TE                                DMA_SxCR_TEIE
#define DMA_IT_DME                               DMA_SxCR_DMEIE
#define DMA_IT_FE                                0x00000080U

#define DMA_PINC_ENABLE                          DMA_SxCR_PINC    /*!< Peripheral increment mode enable  */
#define DMA_PINC_DISABLE                         0x00000000U      /*!< Peripheral increment mode disable */

#define DMA_MINC_ENABLE                          DMA_SxCR_MINC    /*!< Memory increment mode enable  */
#define DMA_MINC_DISABLE                         0x00000000U      /*!< Memory increment mode disable */

#define DMA_PDATAALIGN_BYTE                      0x00000000U        /*!< Peripheral data alignment: Byte     */
#define DMA_PDATAALIGN_HALFWORD                  DMA_SxCR_PSIZE_0   /*!< Peripheral data alignment: HalfWord */
#define DMA_PDATAALIGN_WORD                      DMA_SxCR_PSIZE_1   /*!< Peripheral data alignment: Word     */

#define DMA_MDATAALIGN_BYTE                      0x00000000U        /*!< Memory data alignment: Byte     */
#define DMA_MDATAALIGN_HALFWORD                  DMA_SxCR_MSIZE_0   /*!< Memory data alignment: HalfWord */
#define DMA_MDATAALIGN_WORD                      DMA_SxCR_MSIZE_1   /*!< Memory data alignment: Word     */

#define DMA_NORMAL                               0x00000000U       /*!< Normal mode                  */
#define DMA_CIRCULAR                             DMA_SxCR_CIRC     /*!< Circular mode                */
#define DMA_PFCTRL                               DMA_SxCR_PFCTRL   /*!< Peripheral flow control mode */

#define DMA_PRIORITY_LOW                         0x00000000U    /*!< Priority level: Low       */
#define DMA_PRIORITY_MEDIUM                      DMA_SxCR_PL_0  /*!< Priority level: Medium    */
#define DMA_PRIORITY_HIGH                        DMA_SxCR_PL_1  /*!< Priority level: High      */
#define DMA_PRIORITY_VERY_HIGH                   DMA_SxCR_PL    /*!< Priority level: Very High */

#define DMA_PBURST_SINGLE                        0x00000000U
#define DMA_PBURST_INC4                          DMA_SxCR_PBURST_0
#define DMA_PBURST_INC8                          DMA_SxCR_PBURST_1
#define DMA_PBURST_INC16                         DMA_SxCR_PBURST

#define DMA_CHANNEL_0                     0x00000000U  /*!< DMA Channel 0 */
#define DMA_CHANNEL_1                     0x02000000U  /*!< DMA Channel 1 */
#define DMA_CHANNEL_2                     0x04000000U  /*!< DMA Channel 2 */
#define DMA_CHANNEL_3                     0x06000000U  /*!< DMA Channel 3 */
#define DMA_CHANNEL_4                     0x08000000U  /*!< DMA Channel 4 */
#define DMA_CHANNEL_5                     0x0A000000U  /*!< DMA Channel 5 */
#define DMA_CHANNEL_6                     0x0C000000U  /*!< DMA Channel 6 */
#define DMA_CHANNEL_7                     0x0E000000U  /*!< DMA Channel 7 */
#if defined (STM32F722xx) || defined (STM32F723xx) || defined (STM32F732xx) || defined (STM32F733xx) ||\
    defined (STM32F765xx) || defined (STM32F767xx) || defined (STM32F769xx) || defined (STM32F777xx) ||\
    defined (STM32F779xx) || defined (STM32F730xx)
#define DMA_CHANNEL_8                     0x10000000U  /*!< DMA Channel 8 */
#define DMA_CHANNEL_9                     0x12000000U  /*!< DMA Channel 9 */
#define DMA_CHANNEL_10                    0x14000000U  /*!< DMA Channel 10*/
#define DMA_CHANNEL_11                    0x16000000U  /*!< DMA Channel 11*/
#define DMA_CHANNEL_12                    0x18000000U  /*!< DMA Channel 12*/
#define DMA_CHANNEL_13                    0x1A000000U  /*!< DMA Channel 13*/
#define DMA_CHANNEL_14                    0x1C000000U  /*!< DMA Channel 14*/
#define DMA_CHANNEL_15                    0x1E000000U  /*!< DMA Channel 15*/
#endif /* STM32F722xx || STM32F723xx || STM32F732xx || STM32F733xx || STM32F765xx || STM32F767xx ||
          STM32F769xx || STM32F777xx || STM32F779xx || STM32F730xx */

#define DMA_FLAG_FEIF0_4                         0x00000001U
#define DMA_FLAG_DMEIF0_4                        0x00000004U
#define DMA_FLAG_TEIF0_4                         0x00000008U
#define DMA_FLAG_HTIF0_4                         0x00000010U
#define DMA_FLAG_TCIF0_4                         0x00000020U
#define DMA_FLAG_FEIF1_5                         0x00000040U
#define DMA_FLAG_DMEIF1_5                        0x00000100U
#define DMA_FLAG_TEIF1_5                         0x00000200U
#define DMA_FLAG_HTIF1_5                         0x00000400U
#define DMA_FLAG_TCIF1_5                         0x00000800U
#define DMA_FLAG_FEIF2_6                         0x00010000U
#define DMA_FLAG_DMEIF2_6                        0x00040000U
#define DMA_FLAG_TEIF2_6                         0x00080000U
#define DMA_FLAG_HTIF2_6                         0x00100000U
#define DMA_FLAG_TCIF2_6                         0x00200000U
#define DMA_FLAG_FEIF3_7                         0x00400000U
#define DMA_FLAG_DMEIF3_7                        0x01000000U
#define DMA_FLAG_TEIF3_7                         0x02000000U
#define DMA_FLAG_HTIF3_7                         0x04000000U
#define DMA_FLAG_TCIF3_7                         0x08000000U

/**
  * @brief  Enable the specified DMA Stream.
  * @param  __HANDLE__ DMA handle
  * @retval None
  */
#define DMA_ENABLE(__HANDLE__)      ((__HANDLE__)->Instance->CR |=  DMA_SxCR_EN)

/**
  * @brief  Disable the specified DMA Stream.
  * @param  __HANDLE__ DMA handle
  * @retval None
  */
#define DMA_DISABLE(__HANDLE__)     ((__HANDLE__)->Instance->CR &=  ~DMA_SxCR_EN)

/** 
  * @brief  DMA Configuration Structure definition
  */
typedef struct
{
  unsigned int Channel;              /*!< Specifies the channel used for the specified stream. 
                                      This parameter can be a value of @ref DMAEx_Channel_selection                  */

  unsigned int Direction;            /*!< Specifies if the data will be transferred from memory to peripheral, 
                                      from memory to memory or from peripheral to memory.
                                      This parameter can be a value of @ref DMA_Data_transfer_direction              */

  unsigned int PeriphInc;            /*!< Specifies whether the Peripheral address register should be incremented or not.
                                      This parameter can be a value of @ref DMA_Peripheral_incremented_mode          */

  unsigned int MemInc;               /*!< Specifies whether the memory address register should be incremented or not.
                                      This parameter can be a value of @ref DMA_Memory_incremented_mode              */

  unsigned int PeriphDataAlignment;  /*!< Specifies the Peripheral data width.
                                      This parameter can be a value of @ref DMA_Peripheral_data_size                 */

  unsigned int MemDataAlignment;     /*!< Specifies the Memory data width.
                                      This parameter can be a value of @ref DMA_Memory_data_size                     */

  unsigned int Mode;                 /*!< Specifies the operation mode of the DMAy Streamx.
                                      This parameter can be a value of @ref DMA_mode
                                      @note The circular buffer mode cannot be used if the memory-to-memory
                                            data transfer is configured on the selected Stream                        */

  unsigned int Priority;             /*!< Specifies the software priority for the DMAy Streamx.
                                      This parameter can be a value of @ref DMA_Priority_level                       */

  unsigned int FIFOMode;             /*!< Specifies if the FIFO mode or Direct mode will be used for the specified stream.
                                      This parameter can be a value of @ref DMA_FIFO_direct_mode
                                      @note The Direct mode (FIFO mode disabled) cannot be used if the 
                                            memory-to-memory data transfer is configured on the selected stream       */

  unsigned int FIFOThreshold;        /*!< Specifies the FIFO threshold level.
                                      This parameter can be a value of @ref DMA_FIFO_threshold_level                  */

  unsigned int MemBurst;             /*!< Specifies the Burst transfer configuration for the memory transfers. 
                                      It specifies the amount of data to be transferred in a single non interruptible 
                                      transaction.
                                      This parameter can be a value of @ref DMA_Memory_burst 
                                      @note The burst mode is possible only if the address Increment mode is enabled. */

  unsigned int PeriphBurst;          /*!< Specifies the Burst transfer configuration for the peripheral transfers. 
                                      It specifies the amount of data to be transferred in a single non interruptible 
                                      transaction. 
                                      This parameter can be a value of @ref DMA_Peripheral_burst
                                      @note The burst mode is possible only if the address Increment mode is enabled. */
} DMA_InitTypeDef;

typedef struct __DMA_HandleTypeDef
{
  DMA_Stream_TypeDef         *Instance;                                                    /*!< Register base address                  */

  DMA_InitTypeDef            Init;                                                         /*!< DMA communication parameters           */ 

  unsigned int               StreamBaseAddress;                                            /*!< DMA Stream Base Address                */

  unsigned int               StreamIndex;                                                  /*!< DMA Stream Index                       */
 
} DMA_HandleTypeDef;

typedef struct
{
  __IO unsigned int ISR;   /*!< DMA interrupt status register */
  __IO unsigned int Reserved0;
  __IO unsigned int IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;

unsigned int DMA_Init(DMA_HandleTypeDef *hdma);
void DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DstAddress2, uint32_t DataLength);
void DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DstAddress2, uint32_t DataLength);

#endif
