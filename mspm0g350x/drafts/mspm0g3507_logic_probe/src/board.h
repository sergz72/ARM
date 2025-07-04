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
#define UART_IBRD_5_MHZ_115200_BAUD                                     2
#define UART_FBRD_5_MHZ_115200_BAUD                                    45
#define UART_BUFFER_SIZE                                             1024

#define PWM_INSTANCE                                                TIMG6
#define PWM_PORT                                                    GPIOA
#define PWM_PIN                                             DL_GPIO_PIN_5
#define PWM_IOMUX                                           IOMUX_PINCM10
#define PWM_IOMUX_FUNC                        IOMUX_PINCM10_PF_TIMG6_CCP0

#define CLKOUT_PORT                                                 GPIOA
#define CLKOUT_PIN                                         DL_GPIO_PIN_14
#define CLKOUT_IOMUX                                        IOMUX_PINCM36
#define CLKOUT_IOMUX_FUNC                 IOMUX_PINCM36_PF_SYSCTL_CLK_OUT

#define COMP_IN0P_PORT                                              GPIOA
#define COMP_IN0P_PIN                                      DL_GPIO_PIN_15

#define DAC_REFERENCE_VOLTAGE                                        2500
#define DEFAULT_DAC_LOW_VOLTAGE                                       400
#define DEFAULT_DAC_HIGH_VOLTAGE                                     2400

#define COMP_LOW_INSTANCE                                           COMP0
#define COMP_LOW_DACCODE0 (256*DEFAULT_DAC_LOW_VOLTAGE/DAC_REFERENCE_VOLTAGE)
#define COMP_LOW_OUT_PORT                                           GPIOA
#define COMP_LOW_OUT_PIN                                    DL_GPIO_PIN_7
#define COMP_LOW_IOMUX_OUT                                  IOMUX_PINCM14
#define COMP_LOW_IOMUX_OUT_FUNC                IOMUX_PINCM14_PF_COMP0_OUT
#define COMP_LOW_INSTANCE_PUB_CH                                        0

#define COMP_HIGH_INSTANCE                                          COMP1
#define COMP_HIGH_DACCODE0 (256*DEFAULT_DAC_HIGH_VOLTAGE/DAC_REFERENCE_VOLTAGE)
#define COMP_HIGH_OUT_PORT                                          GPIOA
#define COMP_HIGH_OUT_PIN                                   DL_GPIO_PIN_3
#define COMP_HIGH_IOMUX_OUT                                  IOMUX_PINCM8
#define COMP_HIGH_IOMUX_OUT_FUNC                IOMUX_PINCM8_PF_COMP1_OUT
#define COMP_HIGH_INSTANCE_PUB_CH                                       1

#define COUNTER_LOW_INSTANCE                                        TIMA0
#define COUNTER_LOW_IRQHandler                           TIMA0_IRQHandler
#define COUNTER_LOW_IRQN                                   TIMA0_INT_IRQn

#define COUNTER_HIGH_INSTANCE                                       TIMA1
#define COUNTER_HIGH_IRQHandler                          TIMA1_IRQHandler
#define COUNTER_HIGH_IRQN                                  TIMA1_INT_IRQn

#define POWER_STARTUP_DELAY                                            16

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200

#define TIMER_EVENT_FREQUENCY 10

#define USART_INTERRUPT_PRIORITY 2
#define COUNTERS_INTERRUPT_PRIORITY 1
#define TIMER_INTERRUPT_PRIORITY 0

#define SSD1306_128_32
#define LCD_ORIENTATION LCD_ORIENTATION_LANDSCAPE_REVERSED

#include <lcd_ssd1306.h>

#define LCD_PRINTF_BUFFER_LENGTH 30
#define DRAW_TEXT_MAX 20
#define USE_MYVSPRINTF

//#define I2C_TIMEOUT 1000000
//#define SPI_TIMEOUT 1000000

#define WS2812_MAX_LEDS 4
#define WS2812_MAX_VALUE 0x40

extern unsigned int counter_low, counter_high, counter_freq_low, counter_freq_high;

void SystemInit(void);

unsigned int mv_to_8(unsigned int mv);
void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty);
void stop_counters(void);
void start_counters(void);
void set_l_voltage(unsigned int value);
void set_h_voltage(unsigned int value);

#endif
