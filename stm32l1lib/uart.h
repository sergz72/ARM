#ifndef _UART_H
#define _UART_H

/**
  * @brief USART Init Structure definition
  */
typedef struct
{
  unsigned int BaudRate;                  /*!< This member configures the UART communication baud rate.
                                           The baud rate is computed using the following formula:
                                           - IntegerDivider = ((PCLKx) / (8 * (OVR8+1) * (huart->Init.BaudRate)))
                                           - FractionalDivider = ((IntegerDivider - ((unsigned int) IntegerDivider)) * 8 * (OVR8+1)) + 0.5
                                           Where OVR8 is the "oversampling by 8 mode" configuration bit in the CR1 register. */

  unsigned int WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref UART_Word_Length */

  unsigned int StopBits;                  /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref UART_Stop_Bits */

  unsigned int Parity;                    /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref UART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */

  unsigned int Mode;                      /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Mode */

  unsigned int HwFlowCtl;                 /*!< Specifies whether the hardware flow control mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Hardware_Flow_Control */

  unsigned int OverSampling;              /*!< Specifies whether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to fPCLK/8).
                                           This parameter can be a value of @ref UART_Over_Sampling */
} UART_InitTypeDef;

#define UART_MODE_RX                USART_CR1_RE
#define UART_MODE_TX                USART_CR1_TE
#define UART_MODE_TX_RX             (USART_CR1_TE | USART_CR1_RE)

#define UART_PARITY_NONE            0x00000000U
#define UART_PARITY_EVEN            USART_CR1_PCE
#define UART_PARITY_ODD             (USART_CR1_PCE | USART_CR1_PS)

#define UART_STOPBITS_1             0x00000000U
#define UART_STOPBITS_2             USART_CR2_STOP_1

#define UART_WORDLENGTH_8B          0x00000000U
#define UART_WORDLENGTH_9B          USART_CR1_M

#define UART_HWCONTROL_NONE                  0x00000000U
#define UART_HWCONTROL_RTS                   USART_CR3_RTSE
#define UART_HWCONTROL_CTS                   USART_CR3_CTSE
#define UART_HWCONTROL_RTS_CTS               (USART_CR3_RTSE | USART_CR3_CTSE)

#define UART_OVERSAMPLING_16                    0x00000000U
#define UART_OVERSAMPLING_8                     USART_CR1_OVER8

void UART_Init(USART_TypeDef *instance, UART_InitTypeDef *init);
unsigned int UART_Send(USART_TypeDef *instance, unsigned char c);
unsigned int UART_Receive(USART_TypeDef *instance, unsigned char *c);

#endif
