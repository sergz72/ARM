#include <board.h>
#include <dma.h>

void DMA_ChannelEnable(DMA_Channel_TypeDef *channel)
{
  channel->CCR |= DMA_CCR_EN;
}

void DMA_ChannelDisable(DMA_Channel_TypeDef *channel)
{
  channel->CCR &= ~DMA_CCR_EN;
}
