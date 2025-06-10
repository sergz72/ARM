#include "board.h"
#include <systick.h>
#include <pll.h>
#include <dbgmcu.h>
#include <usb.h>
#include <nvic.h>
#include <gpio.h>
#include <i2c.h>
#include <spi.h>
#include <dma.h>
#include <timer.h>
#include <nvic.h>
#include "sound.h"

volatile int Sys_Tick;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
  systick_set_reload(us * SYSTICK_MULTIPLIER);

  systick_interrupt_enable();
  /* start counting */
  systick_counter_enable();
}

void systick_stop(void)
{
  /* stop counting */
  systick_counter_disable();
  systick_interrupt_disable();
}

void systick_wait(void)
{
  Sys_Tick = 0;
  while (!Sys_Tick)
    __WFI();
}

void delay(unsigned int us)
{
  systick_start(us);
  systick_wait();
  systick_stop();
}

void delayms(unsigned int ms)
{
  while (ms--)
    delay(1000);
}

int cs43l22_read(unsigned char register_number, unsigned char *value)
{
  return I2C_Transfer(I2C1, CS43L22_ADDRESS, &register_number, 1, value, 1, I2C_TIMEOUT);
}

int cs43l22_write(unsigned char register_number, unsigned char value)
{
  unsigned char data[2];
  data[0] = register_number;
  data[1] = value;
  return I2C_Transfer(I2C1, CS43L22_ADDRESS, data, 2, NULL, 0, I2C_TIMEOUT);
}

static void GPIOInit(void)
{
  //LEDs
  GPIO_Init(GPIOD,
            GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  //CS43L22 reset pin
  GPIO_Init(CS43L22_RESET_GPIO,
            CS43L22_RESET_PIN,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

static void USB_Init(void)
{
  //PA11=USBDM,PA12=USBDP
  GPIO_Init(GPIOA,
            GPIO_Pin_11 | GPIO_Pin_12,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG_FS) ;
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG_FS) ;

  RCC->AHB2ENR |= RCC_AHB2LPENR_OTGFSLPEN;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_Init(OTG_FS_IRQn, 1, 3, ENABLE);
}

static void I2S3Init(void)
{
  I2S_InitStruct init;

  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

  //PC7=MCK
  GPIO_Init(GPIOC,
            GPIO_Pin_7,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  //PC10=CK, PC12=SD
  /*GPIO_Init(GPIOC,
            GPIO_Pin_10 | GPIO_Pin_12,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  //PA4=WS
  GPIO_Init(GPIOA,
            GPIO_Pin_4,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );*/
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_SPI3); // mclk
  //GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_I2S3ext);
  //GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_I2S3ext);
  //GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_I2S3ext);

  init.i2sdiv = 7;
  init.channel_32bit = 0;
  init.clock_polarity = 0;
  init.long_frame_sync = 0;
  init.master_clock_output_enable = 1;
  init.mode = I2S_MODE_MASTER_TX;
  init.data_size = I2S_DATA_SIZE_16BIT;
  init.standard = I2S_STANDARD_PHILIPS;
  I2S_Init(SPI3, &init);
  I2S_Enable(SPI3);
}

static void I2C1Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  //PB6=SCL,PB9=SDA
  GPIO_Init(GPIOB,
            GPIO_Pin_6 | GPIO_Pin_9,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_OD,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);

  I2C_Master_Init(I2C1, 100000);
  I2C_Enable(I2C1);
}

static void DACInit(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;

  //PA4 = DAC output
  GPIO_Init(GPIOA,
            GPIO_Pin_4,
            GPIO_Mode_AN,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  // enable DAC
  // DAC trigger is enabled
  // DAC trigger source = Timer6
  // DAC DMA is enabled
  DAC->CR = DAC_CR_EN1 | DAC_CR_TEN1 | DAC_CR_DMAEN1 | DAC_CR_BOFF1;
}

typedef struct
{
  uint32_t channel;
  DMA_Stream_TypeDef *stream;
  uint32_t peripheral_address;
  uint32_t peripheral_data_size;
  uint32_t memory_data_size;
} DMA_InitStruct;

static void DMAInit(const DMA_InitStruct *config)
{
  DMA_InitTypeDef init;

  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  init.DMA_Mode = DMA_Mode_Circular;
  init.DMA_Channel = config->channel;
  init.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  init.DMA_FIFOMode = DMA_FIFOMode_Disable;
  init.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  init.DMA_Memory0BaseAddr = (uint32_t)sound_out_buffer;
  init.DMA_MemoryDataSize = config->memory_data_size;
  init.DMA_MemoryInc = DMA_MemoryInc_Enable;
  init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  init.DMA_PeripheralBaseAddr = config->peripheral_address;
  init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  init.DMA_PeripheralDataSize = config->peripheral_data_size;
  init.DMA_Priority = DMA_Priority_VeryHigh;
  init.DMA_BufferSize = SOUND_OUT_BUFFER_SIZE;
  DMA_Init(config->stream, &init);
  DMA_FlowControllerConfig(config->stream, DMA_FlowCtrl_Memory);
  // half transfer interrupt
  DMA_ITConfig(config->stream, DMA_IT_HT, ENABLE);
  // transfer complete interrupt
  DMA_ITConfig(config->stream, DMA_IT_TC, ENABLE);
}

static void DMAInitForDac(void)
{
  DMA_InitStruct config;
  config.channel = DMA_Channel_7;
  config.stream = DMA1_Stream5;
  config.peripheral_address = (uint32_t)&DAC->DHR12R1;
  config.peripheral_data_size = DMA_PeripheralDataSize_HalfWord;
  config.memory_data_size = DMA_MemoryDataSize_HalfWord;
  DMAInit(&config);
  NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

static void Timer6Init(void)
{
  TIM_TimeBaseInitTypeDef init;

  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

  init.TIM_Prescaler = 0;
  init.TIM_CounterMode = TIM_CounterMode_Up;
  init.TIM_Period = PCLK1_FREQ / SAMPLE_RATE - 1;
  init.TIM_ClockDivision = TIM_CKD_DIV1;
  init.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM6, &init);
  TIM_ARRPreloadConfig(TIM6, ENABLE);
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
}

void SystemInit(void)
{
  /* Set floating point coprosessor access mode. */
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

#ifdef DEBUG
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif

  // Init system clock (48 MHz)
  InitRCC(8, 48, FLASH_ACR_LATENCY_1WS, RCC_CFGR_HPRE_DIV1,
    RCC_CFGR_PPRE2_DIV1, RCC_CFGR_PPRE1_DIV2, 0);
  InitPLL_I2S(86000000, 86000000); // i2s divider = 25, audio frequency=48,000, mclk = 12,288,000

  /* 48MHz / 8 => 6M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  //enable the GPIO clock for port GPIOA
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  //enable the GPIO clock for port GPIOD
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  GPIOInit();
  USB_Init();
  I2S3Init();
  I2C1Init();
  DACInit();
  DMAInitForDac();
  Timer6Init();
}

void OTG_FS_IRQHandler(void)
{
  USBInterruptHandler();
}
