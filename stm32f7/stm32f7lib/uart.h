#ifndef _UART_H
#define _UART_H

typedef struct
{
  unsigned int BaudRate;                  /*!< This member configures the UART communication baud rate.
                                           The baud rate register is computed using the following formula:
                                           - If oversampling is 16 or in LIN mode,
                                              Baud Rate Register = ((uart_ker_ck) / ((huart->Init.BaudRate)))
                                           - If oversampling is 8,
                                              Baud Rate Register[15:4] = ((2 * uart_ker_ck) / ((huart->Init.BaudRate)))[15:4]
                                              Baud Rate Register[3] =  0
                                              Baud Rate Register[2:0] =  (((2 * uart_ker_ck) / ((huart->Init.BaudRate)))[3:0]) >> 1
                                           where uart_ker_ck is the UART input clock */

  unsigned int WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref UARTEx_Word_Length. */

  unsigned int StopBits;                  /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref UART_Stop_Bits. */

  unsigned int Parity;                    /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref UART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */

  unsigned int Mode;                      /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Mode. */

  unsigned int HwFlowCtl;                 /*!< Specifies whether the hardware flow control mode is enabled
                                           or disabled.
                                           This parameter can be a value of @ref UART_Hardware_Flow_Control. */

  unsigned int OverSampling;              /*!< Specifies whether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to f_PCLK/8).
                                           This parameter can be a value of @ref UART_Over_Sampling. */

  unsigned int OneBitSampling;            /*!< Specifies whether a single sample or three samples' majority vote is selected.
                                           Selecting the single sample method increases the receiver tolerance to clock
                                           deviations. This parameter can be a value of @ref UART_OneBit_Sampling. */
} UART_InitTypeDef;

#define UART_OVERSAMPLING_16                0x00000000U         /*!< Oversampling by 16 */
#define UART_OVERSAMPLING_8                 USART_CR1_OVER8     /*!< Oversampling by 8  */

#define UART_WORDLENGTH_7B                  USART_CR1_M1   /*!< 7-bit long UART frame */
#define UART_WORDLENGTH_8B                  0x00000000U    /*!< 8-bit long UART frame */
#define UART_WORDLENGTH_9B                  USART_CR1_M0   /*!< 9-bit long UART frame */

#define UART_STOPBITS_0_5                    USART_CR2_STOP_0                     /*!< UART frame with 0.5 stop bit  */
#define UART_STOPBITS_1                     0x00000000U                           /*!< UART frame with 1 stop bit    */
#define UART_STOPBITS_1_5                   (USART_CR2_STOP_0 | USART_CR2_STOP_1) /*!< UART frame with 1.5 stop bits */
#define UART_STOPBITS_2                      USART_CR2_STOP_1                     /*!< UART frame with 2 stop bits   */

#define UART_PARITY_NONE                    0x00000000U                        /*!< No parity   */
#define UART_PARITY_EVEN                    USART_CR1_PCE                      /*!< Even parity */
#define UART_PARITY_ODD                     (USART_CR1_PCE | USART_CR1_PS)     /*!< Odd parity  */

#define UART_HWCONTROL_NONE                  0x00000000U                          /*!< No hardware control       */
#define UART_HWCONTROL_RTS                   USART_CR3_RTSE                       /*!< Request To Send           */
#define UART_HWCONTROL_CTS                   USART_CR3_CTSE                       /*!< Clear To Send             */
#define UART_HWCONTROL_RTS_CTS               (USART_CR3_RTSE | USART_CR3_CTSE)    /*!< Request and Clear To Send */

#define UART_MODE_RX                        USART_CR1_RE                    /*!< RX mode        */
#define UART_MODE_TX                        USART_CR1_TE                    /*!< TX mode        */
#define UART_MODE_TX_RX                     (USART_CR1_TE |USART_CR1_RE)    /*!< RX and TX mode */

#define UART_OVERSAMPLING_16                0x00000000U         /*!< Oversampling by 16 */
#define UART_OVERSAMPLING_8                 USART_CR1_OVER8     /*!< Oversampling by 8  */

#define UART_ONE_BIT_SAMPLE_DISABLE         0x00000000U         /*!< One-bit sampling disable */
#define UART_ONE_BIT_SAMPLE_ENABLE          USART_CR3_ONEBIT    /*!< One-bit sampling enable  */

#define UART_TIMEOUT_VALUE              0x1FFFFFFU  /*!< UART polling-based communications time-out value */

unsigned int UART_Init(USART_TypeDef *huart, const UART_InitTypeDef *init);
unsigned int UART_Transmit(USART_TypeDef *huart, unsigned char *pData, unsigned int Size, unsigned int Timeout);
unsigned int UART_Receive(USART_TypeDef *huart, unsigned char *pData);
void UART_Printf(USART_TypeDef *huart, const char *format, ...);

#endif
