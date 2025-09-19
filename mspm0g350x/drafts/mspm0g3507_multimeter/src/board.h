#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define LED_GPIO GPIOA
#define LED_PIN DL_GPIO_PIN_0
#define LED_IOMUX IOMUX_PINCM1

#define CPUCLK_FREQ                                              80000000

#define SYSTICK_MULTIPLIER 80

#define UART_INSTANCE                                               UART0
#define UART_IRQHandler                                  UART0_IRQHandler
#define UART_IRQN                                          UART0_INT_IRQn
#define UART_RX_PORT                                                GPIOA
#define UART_TX_PORT                                                GPIOA
#define UART_RX_PIN                                        DL_GPIO_PIN_11
#define UART_TX_PIN                                        DL_GPIO_PIN_10
#define UART_IOMUX_RX                                       IOMUX_PINCM22
#define UART_IOMUX_TX                                       IOMUX_PINCM21
#define UART_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define UART_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_BAUD_RATE                                             115200
#define UART_IBRD_32_MHZ_115200_BAUD                                   17
#define UART_FBRD_32_MHZ_115200_BAUD                                   23
#define UART_BUFFER_SIZE                                             1024

#define POWER_STARTUP_DELAY                                            16

#define I2C_BUS_SPEED_HZ                                                  400000
#define GPIO_I2C_SDA_PORT                                                  GPIOA
#define GPIO_I2C_SDA_PIN                                          DL_GPIO_PIN_16
#define GPIO_I2C_IOMUX_SDA                                         IOMUX_PINCM38
#define GPIO_I2C_IOMUX_SDA_FUNC                        IOMUX_PINCM38_PF_I2C1_SDA
#define GPIO_I2C_SCL_PORT                                                  GPIOA
#define GPIO_I2C_SCL_PIN                                          DL_GPIO_PIN_17
#define GPIO_I2C_IOMUX_SCL                                         IOMUX_PINCM39
#define GPIO_I2C_IOMUX_SCL_FUNC                        IOMUX_PINCM39_PF_I2C1_SCL

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200

#define USART_INTERRUPT_PRIORITY 2

#define I2C_TIMEOUT 1000000

#define MCP3421_DEVICE_ID 0xD0

#define DAC_REFERENCE_VOLTAGE 1400    // mV
#define ADC_REFERENCE_VOLTAGE 2048000 // uV
#define ADC_MAX_VALUE         0x1FFFF
#define ADC_MIN_VALUE         0xFFFE0000
#define ADC_MAX_GAIN 3

#define EEPROM_START 0x7C00

void SystemInit(void);
unsigned int mv_to_12(unsigned int mv);
int adc_read(int gain, int *value_uv);
unsigned int dac_get(void);
void dac_set(unsigned int value);
int eeprom_write(void *data, unsigned int size);

#endif
