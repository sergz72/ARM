#include <eth.h>
#include <eth_arp.h>
#include <eth_ipv4.h>

#define ARP_REQUEST 0x100
#define ARP_REPLY 0x200

typedef struct __attribute__((__packed__))
{
  unsigned short hardwareType;
  unsigned short protocolType;
  unsigned char  hardwareAddressLength;
  unsigned char  protocolAddressLength;
  unsigned short operationCode;
  ETH_Address sourceMACAddress;
  unsigned short sourceIPAddressLo;
  unsigned short sourceIPAddressHi;
  ETH_Address destMACAddress;
  unsigned short destIPAddressLo;
  unsigned short destIPAddressHi;
} ETH_ARP_Header;

typedef struct __attribute__((__packed__))
{
  ETH_Header eth_hdr;
  ETH_ARP_Header arp_hdr;
} ARP_Reply;

unsigned char arp_table[256][ETH_HWADDR_LEN] __attribute__((section(".ETHQueueSection")));

void ETH_ARP_Handler(ETH_Header *eth_hdr, unsigned int length)
{
  unsigned int dest_ip, i;
  ETH_ARP_Header *arp_header = (ETH_ARP_Header *)((unsigned int)eth_hdr + ETH_HEADER_LENGTH);

  dest_ip = arp_header->destIPAddressLo | ((unsigned int)arp_header->destIPAddressHi << 16);

  //ETH_DEBUG5("ARP packet ", arp_header->operationCode, IP4_ADDR1(dest_ip),
  //           IP4_ADDR2(dest_ip), IP4_ADDR3(dest_ip),
  //           IP4_ADDR4(dest_ip));

  if (arp_header->operationCode == ARP_REQUEST && dest_ip == ETHInstance.Init.ip_addr)
  {
    //ETH_DEBUG("Sending ARP reply...");
    ARP_Reply *reply = (ARP_Reply*)queue_peekTx(&eth_irq_queue);
    for (i = 0; i < ETH_HWADDR_LEN; i++)
      reply->eth_hdr.dest.addr[i] = eth_hdr->src.addr[i];
    for (i = 0; i < ETH_HWADDR_LEN; i++)
      reply->eth_hdr.src.addr[i] = ETHInstance.Init.MACAddr[i];
    reply->eth_hdr.type = eth_hdr->type;
    reply->arp_hdr.hardwareType = arp_header->hardwareType;
    reply->arp_hdr.protocolType = arp_header->protocolType;
    reply->arp_hdr.hardwareAddressLength = arp_header->hardwareAddressLength;
    reply->arp_hdr.protocolAddressLength = arp_header->protocolAddressLength;
    reply->arp_hdr.operationCode = ARP_REPLY;
    for (i = 0; i < ETH_HWADDR_LEN; i++)
      reply->arp_hdr.sourceMACAddress.addr[i] = ETHInstance.Init.MACAddr[i];
    reply->arp_hdr.sourceIPAddressLo = ETHInstance.Init.ip_addr & 0xFFFF;
    reply->arp_hdr.sourceIPAddressHi = ETHInstance.Init.ip_addr >> 16;
    for (i = 0; i < ETH_HWADDR_LEN; i++)
      reply->arp_hdr.destMACAddress.addr[i] = arp_header->sourceMACAddress.addr[i];
    reply->arp_hdr.destIPAddressLo = arp_header->sourceIPAddressLo;
    reply->arp_hdr.destIPAddressHi = arp_header->sourceIPAddressHi;

    queue_add(&eth_irq_queue, sizeof(ARP_Reply));
  }
}

void ETH_UpdateArpTable(unsigned int ip, unsigned char *hwaddr)
{
  unsigned char *dst = (unsigned char*)&arp_table[IP4_ADDR4(ip)];
  *dst++ = *hwaddr++;
  *dst++ = *hwaddr++;
  *dst++ = *hwaddr++;
  *dst++ = *hwaddr++;
  *dst++ = *hwaddr++;
  *dst = *hwaddr;
}

unsigned char *ETH_GetHWAddr(unsigned int ip)
{
  return (unsigned char*)&arp_table[IP4_ADDR4(ip)];
}