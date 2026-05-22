#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define LED_MOTION_GPIO GPIOA
#define LED_MOTION_PIN DL_GPIO_PIN_0
#define LED_MOTION_IOMUX IOMUX_PINCM1

#define LED_TIMER_GPIO GPIOA
#define LED_TIMER_PIN DL_GPIO_PIN_27
#define LED_TIMER_IOMUX IOMUX_PINCM60

#define CPUCLK_FREQ                                               4000000

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
#define UART_IBRD_4_MHZ_115200_BAUD                                     2
#define UART_FBRD_4_MHZ_115200_BAUD                                    11
#define UART_BUFFER_SIZE                                             1024

#define PWM_INSTANCE                                                TIMG6
#define PWM_PORT                                                    GPIOA
#define PWM_PIN                                             DL_GPIO_PIN_5
#define PWM_IOMUX                                           IOMUX_PINCM10
#define PWM_IOMUX_FUNC                        IOMUX_PINCM10_PF_TIMG6_CCP0

#define COMP_IN0P_PORT                                              GPIOA
#define COMP_IN0P_PIN                                      DL_GPIO_PIN_15

#define DAC_REFERENCE_VOLTAGE                                        2500

#define COMP_INSTANCE                                               COMP0
#define COMP_INSTANCE_PUB_CH                                            0

#define CLKOUT_PORT                                                 GPIOA
#define CLKOUT_PIN                                         DL_GPIO_PIN_14
#define CLKOUT_IOMUX                                        IOMUX_PINCM36
#define CLKOUT_IOMUX_FUNC                 IOMUX_PINCM36_PF_SYSCTL_CLK_OUT

#define VREF_VOLTAGE_MV  1400
#define VREF_READY_DELAY 320

#define ADC_INST            ADC1
#define ADC_ADCMEM_0        DL_ADC12_MEM_IDX_0
#define ADC_INST_IRQHandler ADC1_IRQHandler
#define ADC_INST_INT_IRQN   ADC1_INT_IRQn

#define OPA_INST OPA1

#define POWER_STARTUP_DELAY 16

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200

#define TIMER_EVENT_FREQUENCY 10

#define USART_INTERRUPT_PRIORITY 1
#define TIMER_INTERRUPT_PRIORITY 0

#define PIR_SENSOR_FILTER_CRS                     filter_crs
#define PIR_SENSOR_AVERAGING_FILTER_SAMPLES_COUNT 4096
#define PIR_SENSOR_FILTER_THRESHOLD               filter_threshold
#define DAC_DEFAULT_VALUE                         1680
#define MOTION_DETECTOR_ON_TIME                   2

void SystemInit(void);
unsigned int mv_to_8(unsigned int mv);
void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty);
void toggle_timer_led(void);
void set_motion_led(void);
void clear_motion_led(void);
unsigned int dac_get(void);
void dac_set(unsigned int value);

extern volatile unsigned int filter_crs;
extern volatile unsigned short filter_threshold;

#endif
