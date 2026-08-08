#include <eth.h>
#include <eth_arp.h>
#include <eth_ipv4.h>
#include <eth_udp.h>
#include <eth_icmp.h>

const unsigned int BROADCAST_IP = 0xFFFFFFFF;

void ETH_ICMP_Handler(unsigned int destIP, ETH_ICMP_Header *icmp_hdr, unsigned int length);

void ETH_IPV4_Handler(ETH_Header *eth_hdr, unsigned int length)
{
  ETH_IPV4_Header *ipv4_header = (ETH_IPV4_Header *)((unsigned int)eth_hdr + ETH_HEADER_LENGTH);

  ETH_UpdateArpTable(ipv4_header->sourceIP, eth_hdr->src.addr);

  if (!(ipv4_header->flagsAndOffset & 0xFFBF)) // packet is not fragmented
  {
    if (ipv4_header->destIP == ETHInstance.Init.ip_addr)
    {
      //ETH_DEBUG1("ETH_IPV4_Handler", ipv4_header->protocol);
      switch (ipv4_header->protocol)
      {
        case ETH_IP_PROTOCOL_UDP:
          ETH_UDP_Handler(ipv4_header->sourceIP,
                 (ETH_UDP_Header *)((unsigned int)eth_hdr + ETH_HEADER_LENGTH + ETH_IPV4_HEADER_LENGTH));
          break;
        case ETH_IP_PROTOCOL_ICMP:
          ETH_ICMP_Handler(ipv4_header->sourceIP,
                           (ETH_ICMP_Header *)((unsigned int)eth_hdr + ETH_HEADER_LENGTH + ETH_IPV4_HEADER_LENGTH),
                           ETH_Swap(ipv4_header->totalLength) - ETH_IPV4_HEADER_LENGTH);
          break;
      }
    }
    else if (ipv4_header->destIP == BROADCAST_IP)
    {
      switch (ipv4_header->protocol)
      {
        case ETH_IP_PROTOCOL_UDP:
          ETH_UDP_BroadcastHandler(ipv4_header->sourceIP,
                                   (ETH_UDP_Header *)((unsigned int)eth_hdr + ETH_HEADER_LENGTH + ETH_IPV4_HEADER_LENGTH));
          break;
      }
    }
  }
}
