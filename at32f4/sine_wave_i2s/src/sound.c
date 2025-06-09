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
  generator_init(&generator, SAMPLE_RATE, sine_table_16, DEFAULT_TABLE_LENGTH);
  generator_set_frequency(&generator, 440);
  return 0;
}

void I2S_DMA_IRQ_HANDLER(void)
{
  unsigned short *pbuffer = DMA1->sts & DMA1_HDT3_FLAG ? (unsigned short*)sound_out_buffer : (unsigned short*)(sound_out_buffer + SOUND_OUT_BUFFER_SIZE / 2);

  for (int i = 0; i < SOUND_OUT_BUFFER_SIZE / 2; i++)
  {
    unsigned short value = generator_calculate_next_value(&generator);
    *pbuffer++ = value;
    *pbuffer++ = value;
  }

  DMA1->clr = 0xFFFFFFFF;
}
