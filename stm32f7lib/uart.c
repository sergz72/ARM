#include <board.h>
#include <uart.h>
#include <rcc.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/** @brief  Enable UART.
  * @param  __HANDLE__ specifies the UART Handle.
  * @retval None
  */
#define __HAL_UART_ENABLE(__HANDLE__)                   ((__HANDLE__)->CR1 |= USART_CR1_UE)

/** @brief  Disable UART.
  * @param  __HANDLE__ specifies the UART Handle.
  * @retval None
  */
#define __HAL_UART_DISABLE(__HANDLE__)                  ((__HANDLE__)->CR1 &= ~USART_CR1_UE)

#define USART_CR1_FIELDS  ((unsigned int)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | \
                                      USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8 )) /*!< UART or USART CR1 fields of parameters set by UART_SetConfig API */

#define USART_CR3_FIELDS  ((unsigned int)(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT))  /*!< UART or USART CR3 fields of parameters set by UART_SetConfig API */


#define UART_BRR_MIN    0x10U        /* UART BRR minimum authorized value */
#define UART_BRR_MAX    0x0000FFFFU  /* UART BRR maximum authorized value */

#define __HAL_RCC_GET_USART1_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART1SEL)))
#define __HAL_RCC_GET_USART2_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART2SEL)))
#define __HAL_RCC_GET_USART3_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART3SEL)))
#define __HAL_RCC_GET_UART4_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART4SEL)))
#define __HAL_RCC_GET_UART5_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART5SEL)))
#define __HAL_RCC_GET_USART6_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_USART6SEL)))
#define __HAL_RCC_GET_UART7_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART7SEL)))
#define __HAL_RCC_GET_UART8_SOURCE() ((unsigned int)(READ_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_UART8SEL)))

#define UART_DIV_SAMPLING8(__PCLK__, __BAUD__)   ((((__PCLK__)*2U) + ((__BAUD__)/2U)) / (__BAUD__))
#define UART_DIV_SAMPLING16(__PCLK__, __BAUD__)  (((__PCLK__) + ((__BAUD__)/2U)) / (__BAUD__))

#define __HAL_UART_GET_FLAG(__HANDLE__, __FLAG__) (((__HANDLE__)->ISR & (__FLAG__)) == (__FLAG__))

#define UART_FLAG_TEACK                     USART_ISR_TEACK         /*!< UART transmit enable acknowledge flag     */
#define UART_FLAG_RWU                       USART_ISR_RWU           /*!< UART receiver wake-up from mute mode flag */
#define UART_FLAG_SBKF                      USART_ISR_SBKF          /*!< UART send break flag                      */
#define UART_FLAG_CMF                       USART_ISR_CMF           /*!< UART character match flag                 */
#define UART_FLAG_BUSY                      USART_ISR_BUSY          /*!< UART busy flag                            */
#define UART_FLAG_ABRF                      USART_ISR_ABRF          /*!< UART auto Baud rate flag                  */
#define UART_FLAG_ABRE                      USART_ISR_ABRE          /*!< UART auto Baud rate error                 */
#define UART_FLAG_CTS                       USART_ISR_CTS           /*!< UART clear to send flag                   */
#define UART_FLAG_CTSIF                     USART_ISR_CTSIF         /*!< UART clear to send interrupt flag         */
#define UART_FLAG_LBDF                      USART_ISR_LBDF          /*!< UART LIN break detection flag             */
#define UART_FLAG_TXE                       USART_ISR_TXE           /*!< UART transmit data register empty         */
#define UART_FLAG_TC                        USART_ISR_TC            /*!< UART transmission complete                */
#define UART_FLAG_RXNE                      USART_ISR_RXNE          /*!< UART read data register not empty         */
#define UART_FLAG_IDLE                      USART_ISR_IDLE          /*!< UART idle flag                            */
#define UART_FLAG_ORE                       USART_ISR_ORE           /*!< UART overrun error                        */
#define UART_FLAG_NE                        USART_ISR_NE            /*!< UART noise error                          */
#define UART_FLAG_FE                        USART_ISR_FE            /*!< UART frame error                          */
#define UART_FLAG_PE                        USART_ISR_PE            /*!< UART parity error                         */

/** @brief  Report the UART clock source.
  * @param  __HANDLE__ specifies the UART Handle.
  * @param  __CLOCKSOURCE__ output variable.
  * @retval UART clocking source, written in __CLOCKSOURCE__.
  */
#define UART_GETCLOCKSOURCE(__HANDLE__,__CLOCKSOURCE__) \
  do {                                                        \
    if((__HANDLE__) == USART1)                      \
    {                                                         \
       switch(__HAL_RCC_GET_USART1_SOURCE())                  \
       {                                                      \
        case RCC_USART1CLKSOURCE_PCLK2:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK2;         \
          break;                                              \
        case RCC_USART1CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART1CLKSOURCE_SYSCLK:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_USART1CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else if((__HANDLE__) == USART2)                 \
    {                                                         \
       switch(__HAL_RCC_GET_USART2_SOURCE())                  \
       {                                                      \
        case RCC_USART2CLKSOURCE_PCLK1:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK1;         \
          break;                                              \
        case RCC_USART2CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART2CLKSOURCE_SYSCLK:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_USART2CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else if((__HANDLE__) == USART3)                 \
    {                                                         \
       switch(__HAL_RCC_GET_USART3_SOURCE())                  \
       {                                                      \
        case RCC_USART3CLKSOURCE_PCLK1:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK1;         \
          break;                                              \
        case RCC_USART3CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART3CLKSOURCE_SYSCLK:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_USART3CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else if((__HANDLE__) == UART4)                  \
    {                                                         \
       switch(__HAL_RCC_GET_UART4_SOURCE())                   \
       {                                                      \
        case RCC_UART4CLKSOURCE_PCLK1:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK1;         \
          break;                                              \
        case RCC_UART4CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART4CLKSOURCE_SYSCLK:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_UART4CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else if ((__HANDLE__) == UART5)                 \
    {                                                         \
       switch(__HAL_RCC_GET_UART5_SOURCE())                   \
       {                                                      \
        case RCC_UART5CLKSOURCE_PCLK1:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK1;         \
          break;                                              \
        case RCC_UART5CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART5CLKSOURCE_SYSCLK:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_UART5CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else if((__HANDLE__) == USART6)                 \
    {                                                         \
       switch(__HAL_RCC_GET_USART6_SOURCE())                  \
       {                                                      \
        case RCC_USART6CLKSOURCE_PCLK2:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK2;         \
          break;                                              \
        case RCC_USART6CLKSOURCE_HSI:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_USART6CLKSOURCE_SYSCLK:                      \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_USART6CLKSOURCE_LSE:                         \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else if ((__HANDLE__) == UART7)                 \
    {                                                         \
       switch(__HAL_RCC_GET_UART7_SOURCE())                   \
       {                                                      \
        case RCC_UART7CLKSOURCE_PCLK1:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK1;         \
          break;                                              \
        case RCC_UART7CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART7CLKSOURCE_SYSCLK:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_UART7CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else if ((__HANDLE__) == UART8)                 \
    {                                                         \
       switch(__HAL_RCC_GET_UART8_SOURCE())                   \
       {                                                      \
        case RCC_UART8CLKSOURCE_PCLK1:                        \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_PCLK1;         \
          break;                                              \
        case RCC_UART8CLKSOURCE_HSI:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_HSI;           \
          break;                                              \
        case RCC_UART8CLKSOURCE_SYSCLK:                       \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_SYSCLK;        \
          break;                                              \
        case RCC_UART8CLKSOURCE_LSE:                          \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_LSE;           \
          break;                                              \
        default:                                              \
          (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;     \
          break;                                              \
       }                                                      \
    }                                                         \
    else                                                      \
    {                                                         \
      (__CLOCKSOURCE__) = UART_CLOCKSOURCE_UNDEFINED;         \
    }                                                         \
  } while(0U)

typedef enum
{
  UART_CLOCKSOURCE_PCLK1      = 0x00U,    /*!< PCLK1 clock source  */
  UART_CLOCKSOURCE_PCLK2      = 0x01U,    /*!< PCLK2 clock source  */
  UART_CLOCKSOURCE_HSI        = 0x02U,    /*!< HSI clock source    */
  UART_CLOCKSOURCE_SYSCLK     = 0x04U,    /*!< SYSCLK clock source */
  UART_CLOCKSOURCE_LSE        = 0x08U,    /*!< LSE clock source       */
  UART_CLOCKSOURCE_UNDEFINED  = 0x10U     /*!< Undefined clock source */
} UART_ClockSourceTypeDef;

static unsigned int UART_CheckIdleState(USART_TypeDef *huart);
static void UART_SetConfig(USART_TypeDef *huart, const UART_InitTypeDef *init);

unsigned int UART_Init(USART_TypeDef *huart, const UART_InitTypeDef *init)
{
  /* Disable the Peripheral */
  __HAL_UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(huart, init);

  /* In asynchronous mode, the following bits must be kept cleared:
  - LINEN and CLKEN bits in the USART_CR2 register,
  - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* Enable the Peripheral */
  __HAL_UART_ENABLE(huart);

  /* TEACK and/or REACK to check before moving huart->gState and huart->RxState to Ready */
  return UART_CheckIdleState(huart);
}

/**
  * @brief Configure the UART peripheral.
  * @param huart UART handle.
  * @retval none
  */
static void UART_SetConfig(USART_TypeDef *huart, const UART_InitTypeDef *init)
{
  unsigned int tmpreg;
  unsigned int brrtemp;
  UART_ClockSourceTypeDef clocksource;
  unsigned int usartdiv                   = 0x00000000U;

  /*-------------------------- USART CR1 Configuration -----------------------*/
  /* Clear M, PCE, PS, TE, RE and OVER8 bits and configure
  *  the UART Word Length, Parity, Mode and oversampling:
  *  set the M bits according to huart->Init.WordLength value
  *  set PCE and PS bits according to huart->Init.Parity value
  *  set TE and RE bits according to huart->Init.Mode value
  *  set OVER8 bit according to huart->Init.OverSampling value */
  tmpreg = init->WordLength | init->Parity | init->Mode | init->OverSampling;
  MODIFY_REG(huart->CR1, USART_CR1_FIELDS, tmpreg);

  /*-------------------------- USART CR2 Configuration -----------------------*/
  /* Configure the UART Stop Bits: Set STOP[13:12] bits according
  * to huart->Init.StopBits value */
  MODIFY_REG(huart->CR2, USART_CR2_STOP, init->StopBits);

  /*-------------------------- USART CR3 Configuration -----------------------*/
  /* Configure
  * - UART HardWare Flow Control: set CTSE and RTSE bits according
  *   to huart->Init.HwFlowCtl value
  * - one-bit sampling method versus three samples' majority rule according
  *   to huart->Init.OneBitSampling (not applicable to LPUART) */
  tmpreg = init->HwFlowCtl;

  tmpreg |= init->OneBitSampling;
  MODIFY_REG(huart->CR3, USART_CR3_FIELDS, tmpreg);

  /*-------------------------- USART BRR Configuration -----------------------*/
  UART_GETCLOCKSOURCE(huart, clocksource);

  if (init->OverSampling == UART_OVERSAMPLING_8)
  {
    switch (clocksource)
    {
      case UART_CLOCKSOURCE_PCLK1:
        usartdiv = (uint16_t)(UART_DIV_SAMPLING8(PCLK1_FREQ, init->BaudRate));
        break;
      case UART_CLOCKSOURCE_PCLK2:
        usartdiv = (uint16_t)(UART_DIV_SAMPLING8(PCLK2_FREQ, init->BaudRate));
        break;
      case UART_CLOCKSOURCE_HSI:
        usartdiv = (uint16_t)(UART_DIV_SAMPLING8(HSI_VALUE, init->BaudRate));
        break;
      case UART_CLOCKSOURCE_SYSCLK:
        usartdiv = (uint16_t)(UART_DIV_SAMPLING8(SYSCLK_FREQ, init->BaudRate));
        break;
      case UART_CLOCKSOURCE_LSE:
        usartdiv = (uint16_t)(UART_DIV_SAMPLING8(LSE_VALUE, init->BaudRate));
        break;
      case UART_CLOCKSOURCE_UNDEFINED:
      default:
        while (1);
    }

    /* USARTDIV must be greater than or equal to 0d16 */
    if ((usartdiv >= UART_BRR_MIN) && (usartdiv <= UART_BRR_MAX))
    {
      brrtemp = usartdiv & 0xFFF0U;
      brrtemp |= (usartdiv & 0x000FU) >> 1U;
      huart->BRR = brrtemp;
    }
    else
    {
      while (1);
    }
  }
  else
  {
    switch (clocksource)
    {
      case UART_CLOCKSOURCE_PCLK1:
        usartdiv = UART_DIV_SAMPLING16(PCLK1_FREQ, init->BaudRate);
        break;
      case UART_CLOCKSOURCE_PCLK2:
        usartdiv = UART_DIV_SAMPLING16(PCLK2_FREQ, init->BaudRate);
        break;
      case UART_CLOCKSOURCE_HSI:
        usartdiv = UART_DIV_SAMPLING16(HSI_VALUE, init->BaudRate);
        break;
      case UART_CLOCKSOURCE_SYSCLK:
        usartdiv = UART_DIV_SAMPLING16(SYSCLK_FREQ, init->BaudRate);
        break;
      case UART_CLOCKSOURCE_LSE:
        usartdiv = UART_DIV_SAMPLING16(LSE_VALUE, init->BaudRate);
        break;
      case UART_CLOCKSOURCE_UNDEFINED:
      default:
        while (1);
    }

    /* USARTDIV must be greater than or equal to 0d16 */
    if ((usartdiv >= UART_BRR_MIN) && (usartdiv <= UART_BRR_MAX))
    {
      huart->BRR = usartdiv;
    }
    else
    {
      while (1);
    }
  }
}

/**
  * @brief  Handle UART Communication Timeout.
  * @param huart     UART handle.
  * @param Flag      Specifies the UART flag to check
  * @param Status    Flag status (SET or RESET)
  * @param Tickstart Tick start value
  * @param Timeout   Timeout duration
  * @retval HAL status
  */
static unsigned int UART_WaitOnFlagUntilTimeout(USART_TypeDef *huart, unsigned int Flag, FlagStatus Status, unsigned int Timeout)
{
  /* Wait until flag is set */
  while ((__HAL_UART_GET_FLAG(huart, Flag) ? SET : RESET) == Status && Timeout--)
  {
  }

  if (!Timeout)
  {
    /* Disable TXE, RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts for the interrupt process */
    CLEAR_BIT(huart->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE | USART_CR1_TXEIE));
    CLEAR_BIT(huart->CR3, USART_CR3_EIE);

    return 1;
  }

  return 0;
}

/**
  * @brief Check the UART Idle State.
  * @param huart UART handle.
  * @retval HAL status
  */
static unsigned int UART_CheckIdleState(USART_TypeDef *huart)
{
  /* Check if the Transmitter is enabled */
  if ((huart->CR1 & USART_CR1_TE) == USART_CR1_TE)
  {
    /* Wait until TEACK flag is set */
    return UART_WaitOnFlagUntilTimeout(huart, USART_ISR_TEACK, RESET, UART_TIMEOUT_VALUE);
  }

  return 0;
}

/**
  * @brief Send an amount of data in blocking mode.
  * @param huart   UART handle.
  * @param pData   Pointer to data buffer.
  * @param Size    Amount of data to be sent.
  * @param Timeout Timeout duration.
  * @retval HAL status
  */
unsigned int UART_Transmit(USART_TypeDef *huart, unsigned char *pData, unsigned int Size, unsigned int Timeout)
{
  while (Size--)
  {
    if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TXE, RESET, Timeout))
    {
      return 1;
    }
    huart->TDR = *pData++;
  }

  if (UART_WaitOnFlagUntilTimeout(huart, UART_FLAG_TC, RESET, Timeout))
  {
    return 1;
  }

  return 0;
}

/**
  * @brief Receive an amount of data in blocking mode.
  * @param huart   UART handle.
  * @param pData   Pointer to data buffer.
  * @param Size    Amount of data to be received.
  * @param Timeout Timeout duration.
  * @retval HAL status
  */
unsigned int UART_Receive(USART_TypeDef *huart, unsigned char *pData)
{
  /* as long as data have to be received */
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE) == RESET)
  {
    return 1;
  }
  *pData = huart->RDR & 0xFF;
  return 0;
}

void UART_Printf(USART_TypeDef *huart, const char *format, ...)
{
  static char buffer[200 + 1];
  va_list vArgs;
  unsigned int length;

  va_start(vArgs, format);
  vsprintf(buffer, format, vArgs);
  va_end(vArgs);
  length = strlen(buffer);
  UART_Transmit(huart, buffer, length, UART_TIMEOUT_VALUE);
}
