/* includes ------------------------------------------------------------------*/
#include "audio_codec.h"
#include "audio_conf.h"
#include <string.h>

audio_codec_type audio_codec;
uint16_t spk_dma_buffer[DMA_BUFFER_SIZE];

/**
  * @brief  audio codec set microphone freq
  * @param  freq: freq
  * @retval none
  */
void audio_codec_set_mic_freq(uint32_t freq)
{
}

/**
  * @brief  audio codec set speaker freq
  * @param  freq: freq
  * @retval none
  */
void audio_codec_set_spk_freq(uint32_t freq)
{
}


/**
  * @brief  audio codec speaker alt setting config
  * @param  none
  * @retval none
  */
void audio_codec_spk_alt_setting(uint32_t alt_seting)
{
  if(alt_seting == 0)
  {
  }
  else
  {
  }
}

/**
  * @brief  audio codec microphone alt setting config
  * @param  none
  * @retval none
  */
void audio_codec_mic_alt_setting(uint32_t alt_seting)
{
}

/**
  * @brief  audio codec set microphone mute
  * @param  mute: mute state
  * @retval none
  */
void audio_codec_set_mic_mute(uint8_t mute)
{
}


/**
  * @brief  audio codec set speaker mute
  * @param  mute: mute state
  * @retval none
  */
void audio_codec_set_spk_mute(uint8_t mute)
{
  audio_codec.spk_mute = mute;
}


/**
  * @brief  audio codec set microphone volume
  * @param  volume: the new volume
  * @retval none
  */
void audio_codec_set_mic_volume(uint16_t volume)
{
}

/**
  * @brief  audio codec set speaker volume
  * @param  volume: the new volume
  * @retval none
  */
void audio_codec_set_spk_volume(uint16_t volume)
{
  audio_codec.spk_volume = volume;
}

/**
  * @brief  codec speaker feedback
  * @param  feedback: data buffer
  * @retval feedback len
  */
uint8_t audio_codec_spk_feedback(uint8_t *feedback)
{
  uint32_t feedback_value = (audio_codec.spk_freq);
  feedback_value = ((feedback_value/1000)<<14)|((feedback_value%1000)<<4);
  feedback[0] = (uint8_t)(feedback_value);
  feedback[1] = (uint8_t)(feedback_value >> 8);
  feedback[2] = (uint8_t)(feedback_value >> 16);
  return 3;
}

/**
  * @brief  codec speaker write fifo
  * @param  data: data buffer
  * @param  len: data length
  * @retval none
  */
void audio_codec_spk_fifo_write(uint8_t *data, uint32_t len)
{
  uint16_t ulen = len / 2, i;
  uint16_t *u16data = (uint16_t *)data;

  switch(audio_codec.spk_stage)
  {
    case 0:
      audio_codec.spk_woff = audio_codec.spk_roff = audio_codec.spk_buffer;
      audio_codec.spk_wtotal = audio_codec.spk_rtotal = 0;
      audio_codec.spk_stage = 1;
      audio_codec.spk_threshold = SPK_BUFFER_SIZE/2;
      break;
    case 1:
      if( audio_codec.spk_wtotal >= SPK_BUFFER_SIZE/2 )
      {
        audio_codec.spk_stage = 2;
      }
      break;
    case 2:
      break;
  }
  for( i = 0; i < ulen; ++i )
  {
    *audio_codec.spk_woff++ = *u16data++;
    if( audio_codec.spk_woff >= audio_codec.spk_rend )
    {
      audio_codec.spk_woff = audio_codec.spk_buffer;
    }
  }
  audio_codec.spk_wtotal += ulen;
}

/**
  * @brief  codec microphone get data
  * @param  data: data buffer
  * @retval data len
  */
uint32_t audio_codec_mic_get_data(uint8_t *buffer)
{
  return 0;
}

/**
  * @brief  this function handles dma1 channel3 interrupt.
  * @param  none
  * @retval none
  */
void DMA1_Channel3_IRQHandler(void)
{
  uint16_t half_size = audio_codec.spk_tx_size;
  uint16_t *pdst;

  if(dma_interrupt_flag_get(DMA1_HDT3_FLAG) == SET)
  {
    //copy_buff(audio_codec.spk_buffer, audio_codec.spk_tx_fifo + audio_codec.r_pos, half_size);
    pdst = spk_dma_buffer;
    dma_flag_clear(DMA1_HDT3_FLAG);
  }
  else if(dma_interrupt_flag_get(DMA1_FDT3_FLAG) == SET)
  {
    //copy_buff(&audio_codec.spk_buffer[half_size], audio_codec.spk_tx_fifo + audio_codec.r_pos, half_size);
    pdst = spk_dma_buffer + half_size;
    dma_flag_clear(DMA1_FDT3_FLAG);
  }

  switch( audio_codec.spk_stage )
  {
    case 0:
    case 1:
      memset( pdst, 0, half_size << 1);
      break;
    case 2:
      if( audio_codec.spk_wtotal >= audio_codec.spk_rtotal + SPK_BUFFER_SIZE )
      {
        while(1);//should not happen;
      }
      if( audio_codec.spk_rtotal >= audio_codec.spk_wtotal )
      {
        audio_codec.spk_stage = 0;
        audio_codec.spk_woff = audio_codec.spk_roff = audio_codec.spk_buffer;
        audio_codec.spk_rtotal = audio_codec.spk_wtotal = 0;
      }
      else
      {
        memcpy( pdst, audio_codec.spk_roff, half_size << 1 );
        audio_codec.spk_roff   += half_size;
        audio_codec.spk_rtotal += half_size;
        if( audio_codec.spk_roff >= audio_codec.spk_rend )
        {
          audio_codec.spk_roff = audio_codec.spk_buffer;
        }
        if( ++audio_codec.spk_calc == 256 )
        {
          uint16_t delta = audio_codec.spk_wtotal - audio_codec.spk_rtotal;
          audio_codec.spk_calc = 0;
          if( delta < audio_codec.spk_threshold - half_size )
          {
            audio_codec.spk_threshold -= half_size;
            audio_codec.spk_freq += audio_codec.audio_freq/1024;
          }
          else if( delta > audio_codec.spk_threshold + half_size )
          {
            audio_codec.spk_threshold += half_size;
            audio_codec.spk_freq -= audio_codec.audio_freq/1024;
          }
          if(audio_codec.spk_rtotal > 0x20000000)
          {
            audio_codec.spk_rtotal -= 0x10000000;
            audio_codec.spk_wtotal -= 0x10000000;
          }
        }
      }
    break;
  }
}

/**
  * @brief  audio codec init
  * @param  none
  * @retval none
  */
void audio_codec_init(void)
{
  audio_codec.audio_freq = AUDIO_FREQ_16K;
  audio_codec.audio_bitw = AUDIO_BITW_16;
  audio_codec.spk_freq = audio_codec.audio_freq;

  audio_codec.spk_tx_size = (audio_codec.audio_freq / 1000) * (audio_codec.audio_bitw / 8) * AUDIO_SPK_CHANEL_NUM / 2;

  memset(audio_codec.spk_buffer, 0, SPK_BUFFER_SIZE*sizeof(uint16_t));

  audio_codec.spk_rend = audio_codec.spk_woff = audio_codec.spk_roff = audio_codec.spk_buffer;

  while(audio_codec.spk_rend < audio_codec.spk_buffer+SPK_BUFFER_SIZE)
  {
    audio_codec.spk_rend += audio_codec.spk_tx_size;
  }
  audio_codec.spk_rend -= audio_codec.spk_tx_size;
}

/**
  * @brief  audio codec loop
  * @param  none
  * @retval none
  */
void audio_codec_loop(void)
{
  static uint8_t spk_mute = 0;
  static uint16_t spk_volume = 0;

  if(spk_mute != audio_codec.spk_mute)
  {
    spk_mute = audio_codec.spk_mute;
    //todo
  }

  if(spk_volume != audio_codec.spk_volume)
  {
    spk_volume = audio_codec.spk_volume;
    //todo
  }
}
