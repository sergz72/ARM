#include "board.h"
#include <systick.h>
#include <gpio.h>
#include <dbgmcu.h>
#include <i2c.h>
#include <spi.h>
#include <nvic.h>

static const I2C_InitTypeDef i2c_init = {
    .GeneralCallEnable = 0,
    .ClockStretchingEnable = 1,
    .SlaveByteControlEnable = 0,
    .RXDmaEnable = 0,
    .TXDmaEnable = 0,
    .AnalogFilterEnable = 1,
    .DigitalFilter = I2C_DigitalFilter_Disable,
    .ErrorInterruptsEnable = 1,
    .StopInterruptEnable = 1,
    .NAckInterruptEnable = 0,
    .AddressInterruptEnable = 1,
    .OwnAddress1 = 1,
    .OwnAddress2 = 0,
    .OwnAddress2Mask = 0,
    .Timing = 0x2000090E,
    .TXISHandler = TXISHandler,
    .RXNEHandler = RXNEHandler,
    .STOPHandler = STOPHandler,
    .ADDRHandler = ADDRHandler
};

static const SPI_InitTypeDef spi_init = {
    .BidiMode = 0,
    .CRCEnable = 0,
    .CRC16Bit = 0,
    .RXOnly = 0,
    .SoftwareSlaveManagement = 1,
    .LSBFirst = 0,
    .MasterMode = 1,
    .CPOLHigh = 0,
    .CPHA2Edge = 0,
    .FIFOThreshold4 = 0,
    .TIMode = 0,
    .NSSPulse = 0,
    .SSOutputEnable = 0,
    .TXDmaEnable = 0,
    .RXDmaEnable = 0,
    .ErrorInterruptEnable = 0,
    .BaudRateDivider = SPI_DIVIDER8,
    .DataSize = 16
};

static void ClockInit(void)
{
  RCC->CFGR = RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE_DIV1;
}

static void GPIOInit(void)
{
  //LEDs
  GPIO_Init(GPIOC,
            GPIO_Pin_8 | GPIO_Pin_9,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

static void I2C1Init(void)
{
  //SCL
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_1);
  //SDA
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_1);
  GPIO_Init(GPIOB,
            GPIO_Pin_8 | GPIO_Pin_9,
            GPIO_Mode_AF,
            GPIO_Speed_2MHz,
            GPIO_OType_OD,
            GPIO_PuPd_UP
  );

  I2C_Init(I2C1, &i2c_init);
  NVIC_Init(I2C1_IRQn, 3, 1, ENABLE);
}

static void SPI1Init(void)
{
  CS_SET;
  FSY_SET;
  // FSY, CS
  GPIO_Init(GPIOB,
            GPIO_Pin_4 | GPIO_Pin_6,
            GPIO_Mode_OUT,
            GPIO_Speed_50MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  SPI_Init(SPI1, &spi_init);

  //SCK
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_0);
  //MOSI
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_0);
  GPIO_Init(GPIOB,
            GPIO_Pin_3 | GPIO_Pin_5,
            GPIO_Mode_AF,
            GPIO_Speed_50MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

/**
  * @brief  Setup the microcontroller system
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
  ClockInit();

  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB);

  //enable the GPIO clock for port GPIOB
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

#ifdef DEBUG
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif

  GPIOInit();
  I2C1Init();
  SPI1Init();
}
