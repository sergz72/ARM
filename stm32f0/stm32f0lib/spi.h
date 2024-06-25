#ifndef __STM32F0XX_SPI_H
#define __STM32F0XX_SPI_H

#include <stm32f0xx.h>

typedef struct
{
  int BidiMode;
  int CRCEnable;
  int CRC16Bit;
  int RXOnly;
  int SoftwareSlaveManagement;
  int LSBFirst;
  int MasterMode;
  int CPOLHigh;
  int CPHA2Edge;
  int FIFOThreshold4;
  int TIMode;
  int NSSPulse;
  int SSOutputEnable;
  int TXDmaEnable;
  int RXDmaEnable;
  int ErrorInterruptEnable;
  unsigned int BaudRateDivider;
  unsigned int DataSize;
  void (*TXEHandler)(SPI_TypeDef* spi);
  void (*RXNEHandler)(SPI_TypeDef* spi);
  void (*ErrorHandler)(SPI_TypeDef* spi);
} SPI_InitTypeDef;

#define SPI_DIVIDER2   0
#define SPI_DIVIDER4   1
#define SPI_DIVIDER8   2
#define SPI_DIVIDER16  3
#define SPI_DIVIDER32  4
#define SPI_DIVIDER64  5
#define SPI_DIVIDER128 6
#define SPI_DIVIDER256 7

void SPI_Init(SPI_TypeDef* spi, const SPI_InitTypeDef* init);
void SPI_SetCpolCpha(SPI_TypeDef* spi, int cpol, int cpha);
unsigned short SPI_Send(SPI_TypeDef* spi, unsigned short data);
void SPI_BusyWait(SPI_TypeDef* spi);

#endif /*__STM32F0XX_SPI_H */
