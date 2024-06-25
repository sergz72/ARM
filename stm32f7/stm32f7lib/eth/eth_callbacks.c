#include <eth.h>

void ETH_ARP_Handler(ETH_Header *eth_hdr, unsigned int length);
void ETH_IPV4_Handler(ETH_Header *eth_hdr, unsigned int length);

__weak void ETH_UserRxCpltCallback(void)
{
}

void ETH_RxCpltCallback(void)
{
  ETH_Header *eth_hdr;
  unsigned int length, i;
  __IO ETH_DMADescTypeDef *dmarxdesc;

  if (ETH_GetReceivedFrame_IT())
    return;

  ETH_UserRxCpltCallback();

  length = ETHInstance.RxFrameInfos.length;
  eth_hdr = (ETH_Header *)ETHInstance.RxFrameInfos.buffer;

  //ETH_DEBUG2("ETH_RxCpltCallback", length, eth_hdr->type);

  switch (eth_hdr->type)
  {
    case ETH_PROTOCOL_ARP:
      ETH_ARP_Handler(eth_hdr, length);
      break;
    case ETH_PROTOCOL_IPV4:
      ETH_IPV4_Handler(eth_hdr, length);
      break;
  }
  /* Release descriptors to DMA */
  /* Point to first descriptor */
  dmarxdesc = ETHInstance.RxFrameInfos.FSRxDesc;
  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
  for (i=0; i< ETHInstance.RxFrameInfos.SegCount; i++)
  {
    dmarxdesc->Status |= ETH_DMARXDESC_OWN;
    dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
  }

  /* Clear Segment_Count */
  ETHInstance.RxFrameInfos.SegCount = 0;

  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETHInstance.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
  {
    /* Clear RBUS ETHERNET DMA flag */
    ETHInstance.Instance->DMASR = ETH_DMASR_RBUS;
    /* Resume DMA reception */
    ETHInstance.Instance->DMARPDR = 0;
  }
}

void ETH_TxCpltCallback(void)
{

}

void ETH_ErrorCallback(void)
{

}
