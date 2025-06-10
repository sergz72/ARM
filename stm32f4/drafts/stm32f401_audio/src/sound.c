#include "board.h"
#include "sound.h"
#include <string.h>
#include <generator.h>
#include <sine_table_16.h>

unsigned int sound_out_buffer[SOUND_OUT_BUFFER_SIZE];

static SignalGenerator generator;

int sound_init(void)
{
  memset(sound_out_buffer, 0, sizeof(sound_out_buffer));
  generator_init(&generator, 16000, sine_table_16, DEFAULT_TABLE_LENGTH);
  generator_set_frequency(&generator, 440);
  return 0;
}

// SPI3 stream
void __attribute__((section(".RamFunc"))) DMA1_Stream5_IRQHandler(void)
{
  unsigned short *pbuffer = DMA1->HISR & DMA_HISR_HTIF5 ? (unsigned short*)sound_out_buffer : (unsigned short*)sound_out_buffer + SOUND_OUT_BUFFER_SIZE / 2;

  for (int i = 0; i < SOUND_OUT_BUFFER_SIZE / 4; i++)
  {
    unsigned short value = generator_calculate_next_value(&generator);
    *pbuffer++ = value;
    *pbuffer++ = value;
  }

  DMA1->HIFCR = 0xFFFFFFFF;
  DMA1->LIFCR = 0xFFFFFFFF;
}

void __attribute__((section(".RamFunc"))) DMA1_Stream0_IRQHandler(void)
{
  DMA1->HIFCR = 0xFFFFFFFF;
  DMA1->LIFCR = 0xFFFFFFFF;
}

void __attribute__((section(".RamFunc"))) DMA1_Stream1_IRQHandler(void)
{
  DMA1->HIFCR = 0xFFFFFFFF;
  DMA1->LIFCR = 0xFFFFFFFF;
}

void __attribute__((section(".RamFunc"))) DMA1_Stream2_IRQHandler(void)
{
  DMA1->HIFCR = 0xFFFFFFFF;
  DMA1->LIFCR = 0xFFFFFFFF;
}

void __attribute__((section(".RamFunc"))) DMA1_Stream3_IRQHandler(void)
{
  DMA1->HIFCR = 0xFFFFFFFF;
  DMA1->LIFCR = 0xFFFFFFFF;
}

void __attribute__((section(".RamFunc"))) DMA1_Stream4_IRQHandler(void)
{
  DMA1->HIFCR = 0xFFFFFFFF;
  DMA1->LIFCR = 0xFFFFFFFF;
}

void __attribute__((section(".RamFunc"))) DMA1_Stream6_IRQHandler(void)
{
  DMA1->HIFCR = 0xFFFFFFFF;
  DMA1->LIFCR = 0xFFFFFFFF;
}
