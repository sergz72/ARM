#include "board.h"
#include <systick.h>
#include <usb.h>
#include <gpio.h>
#include <nvic.h>
#include <dbgmcu.h>
#include <pll.h>
#include <dma.h>
#include <i2c.h>
#include <spi.h>
#include "sound.h"
#include "tlv320dac3100.h"

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

static void GPIOInit(void)
{
  //LED
  GPIO_Init(GPIOC,
            GPIO_Pin_13,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  LED_OFF;
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

static void DMAInitForI2S(void)
{
  DMA_InitStruct config;
  config.channel = DMA_Channel_0;
  config.stream = DMA1_Stream5;
  config.peripheral_address = (uint32_t)&SPI3->DR;
  config.peripheral_data_size = DMA_PeripheralDataSize_HalfWord;
  config.memory_data_size = DMA_MemoryDataSize_HalfWord;
  DMAInit(&config);
  NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

static void I2C1Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

  //PB6=SCL,PB9=SDA
  GPIO_Init(GPIOB,
            GPIO_Pin_8 | GPIO_Pin_9,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);

  I2C_Master_Init(I2C1, 100000);
  I2C_Enable(I2C1);
}

static void I2S3Init(void)
{
  I2S_InitStruct init;

  RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

  //PB3=CK, PB5=SD
  GPIO_Init(GPIOB,
            GPIO_Pin_3 | GPIO_Pin_5,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  //PA15=WS
  GPIO_Init(GPIOA,
            GPIO_Pin_15,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_SPI3);

  init.i2sdiv = 125;
  init.channel_32bit = 0;
  init.clock_polarity = 0;
  init.long_frame_sync = 0;
  init.master_clock_output_enable = 0;
  init.mode = I2S_MODE_MASTER_TX;
  init.data_size = I2S_DATA_SIZE_16BIT;
  init.standard = I2S_STANDARD_PHILIPS;
  I2S_Init(SPI3, &init);
  SPI3->CR2 = SPI_CR2_TXDMAEN;
  I2S_Enable(SPI3);
}

static void MCOInit(void)
{
  //PA8=MCO1
  GPIO_Init(GPIOA,
            GPIO_Pin_8,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_MCO);

  RCC->CFGR |= RCC_CFGR_MCO1_1; //HSE output clock on PA8
}

static void TLVInit(void)
{
  // TLV reset pin
  GPIO_Init(GPIOB,
            GPIO_Pin_7,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

void SystemInit(void)
{
  /* Set floating point coprosessor access mode. */
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

#ifdef DEBUG
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif

  // Init system clock (48 MHz)
  InitRCC(25, 48, FLASH_ACR_LATENCY_1WS, RCC_CFGR_HPRE_DIV1, RCC_CFGR_PPRE2_DIV2, RCC_CFGR_PPRE1_DIV2, 0);
  InitPLL_I2S(64000000, 64000000); // i2s divider = 125, audio frequency=16000

  /* 48MHz / 8 => 6M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  //enable the GPIO clock for port GPIOA
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  GPIOInit();
  USB_Init();
  I2S3Init();
  I2C1Init();
  //MCOInit();
  DMAInitForI2S();
  TLVInit();
}

int tlv320dac3100_read(unsigned char register_number, unsigned char *value)
{
  return I2C_Transfer(I2C1, TLV320DAC3100_I2C_ADDRESS, &register_number, 1, value, 1, I2C_TIMEOUT);
}

int tlv320dac3100_write(unsigned char register_number, unsigned char value)
{
  unsigned char data[2];
  data[0] = register_number;
  data[1] = value;
  return I2C_Transfer(I2C1, TLV320DAC3100_I2C_ADDRESS, data, 2, NULL, 0, I2C_TIMEOUT);
}

void OTG_FS_IRQHandler(void)
{
  USBInterruptHandler();
}
