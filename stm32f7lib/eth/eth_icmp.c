#include <eth.h>
#include <eth_ipv4.h>
#include <eth_icmp.h>
#include <eth_arp.h>
#include <memory.h>

typedef struct __attribute__((__packed__))
{
  ETH_Header eth_hdr;
  ETH_IPV4_Header ipv4_hdr;
  ETH_ICMP_Header icmp_hdr;
} ICMP_Header;

void ETH_ICMP_Handler(unsigned int destIP, ETH_ICMP_Header *icmp_hdr, unsigned int length)
{
  ICMP_Header *header;
  unsigned char *hwAddr;
  unsigned int i;

  if (icmp_hdr->type == 8 && icmp_hdr->code == 0) // ICMP echo request
  {
    header = (ICMP_Header *)queue_peekTx(&eth_irq_queue);
    hwAddr = ETH_GetHWAddr(destIP);

    for (i = 0; i < ETH_HWADDR_LEN; i++)
    {
      header->eth_hdr.src.addr[i] = ETHInstance.Init.MACAddr[i];
      header->eth_hdr.dest.addr[i] = *hwAddr++;
    }
    header->eth_hdr.type = ETH_PROTOCOL_IPV4;
    header->ipv4_hdr.versionAndHeaderLength = 0x45;
    header->ipv4_hdr.serviceType = 0;
    header->ipv4_hdr.totalLength = ETH_Swap(ETH_IPV4_HEADER_LENGTH + length);
    header->ipv4_hdr.identification = 0;
    header->ipv4_hdr.flagsAndOffset = 0;
    header->ipv4_hdr.ttl = 1;
    header->ipv4_hdr.protocol = ETH_IP_PROTOCOL_ICMP;
    header->ipv4_hdr.headerChecksum = 0;
    header->ipv4_hdr.sourceIP = ETHInstance.Init.ip_addr;
    header->ipv4_hdr.destIP = destIP;

    memcpy(&header->icmp_hdr, icmp_hdr, length);

    header->icmp_hdr.type = 0; // ICMP echo reply
    header->icmp_hdr.checksum = 0;

    queue_add(&eth_irq_queue, length + sizeof(ICMP_Header) - ETH_ICMP_HEADER_LENGTH);
  }
}
