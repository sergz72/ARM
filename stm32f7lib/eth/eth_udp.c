#include <eth_ipv4.h>
#include <eth_udp.h>
#include <eth_arp.h>
#include <memory.h>

typedef struct __attribute__((__packed__))
{
  ETH_Header eth_hdr;
  ETH_IPV4_Header ipv4_hdr;
  ETH_UDP_Header udp_hdr;
} UDP_Header;

typedef struct __attribute__((__packed__))
{
  unsigned char opcode;
  unsigned char hwtype;
  unsigned char hw_address_length;
  unsigned char hop_count;
  unsigned int transaction_id;
  unsigned short number_of_seconds;
  unsigned short flags;
  unsigned int client_ip;
  unsigned int your_ip;
  unsigned int server_ip;
  unsigned int gateway_ip;
  unsigned char client_hardware_address[16];
  unsigned char server_host_name[64];
  unsigned char boot_file_name[128];
  unsigned char magic[4];
  unsigned char options[17];
} ETH_DHCP_Header;

typedef struct __attribute__((__packed__))
{
  ETH_Header eth_hdr;
  ETH_IPV4_Header ipv4_hdr;
  ETH_UDP_Header udp_hdr;
  ETH_DHCP_Header dhcp_hdr;
} DHCP_Header;

void ETH_UDP_Send(unsigned short sourcePort, unsigned int destIP, unsigned short destPort, void *data, unsigned int data_length)
{
  UDP_Header *header = ( UDP_Header *)queue_peekTx(&eth_user_queue);
  unsigned char *hwAddr = ETH_GetHWAddr(destIP);
  unsigned int i;

  for (i = 0; i < ETH_HWADDR_LEN; i++)
  {
    header->eth_hdr.src.addr[i] = ETHInstance.Init.MACAddr[i];
    header->eth_hdr.dest.addr[i] = *hwAddr++;
  }
  header->eth_hdr.type = ETH_PROTOCOL_IPV4;
  header->ipv4_hdr.versionAndHeaderLength = 0x45;
  header->ipv4_hdr.serviceType = 0;
  header->ipv4_hdr.totalLength = ETH_Swap(ETH_IPV4_HEADER_LENGTH + ETH_UDP_HEADER_LENGTH + data_length);
  header->ipv4_hdr.identification = 0;
  header->ipv4_hdr.flagsAndOffset = 0;
  header->ipv4_hdr.ttl = 1;
  header->ipv4_hdr.protocol = ETH_IP_PROTOCOL_UDP;
  header->ipv4_hdr.headerChecksum = 0;
  header->ipv4_hdr.sourceIP = ETHInstance.Init.ip_addr;
  header->ipv4_hdr.destIP = destIP;
  header->udp_hdr.source_port = sourcePort;
  header->udp_hdr.dest_port = destPort;
  header->udp_hdr.length = ETH_Swap(ETH_UDP_HEADER_LENGTH + data_length);
  header->udp_hdr.checksum = 0; // disabled

  memcpy((unsigned char*)header + sizeof(UDP_Header), data, data_length);

  queue_add(&eth_user_queue, data_length + sizeof(UDP_Header));
}

void ETH_UDP_BroadcastHandler(unsigned int sourceIP, ETH_UDP_Header *udp_hdr)
{
  unsigned int i;

  if (udp_hdr->source_port == 0x4300 && udp_hdr->dest_port == 0x4400) //DHCP
  {
    ETH_DHCP_Header *dhcp_header = (ETH_DHCP_Header *)((unsigned int)udp_hdr + ETH_UDP_HEADER_LENGTH);
    for (i = 0; i < ETH_HWADDR_LEN; i++)
    {
      if (dhcp_header->client_hardware_address[i] != ETHInstance.Init.MACAddr[i])
        return;
    }
    //ETH_DEBUG4("IP:", IP4_ADDR1(dhcp_header->your_ip), IP4_ADDR2(dhcp_header->your_ip),
    //   IP4_ADDR3(dhcp_header->your_ip), IP4_ADDR4(dhcp_header->your_ip));
    ETHInstance.Init.ip_addr = dhcp_header->your_ip;
  }
}

void ETH_DHCP_Send(unsigned char *hostName)
{
  DHCP_Header *header = (DHCP_Header *)queue_peekTx(&eth_user_queue);

  unsigned int i, hostnameLength = strlen(hostName);

  memset(&header->dhcp_hdr, 0, sizeof(ETH_DHCP_Header));

  for (i = 0; i < ETH_HWADDR_LEN; i++)
  {
    header->eth_hdr.src.addr[i] = ETHInstance.Init.MACAddr[i];
    header->eth_hdr.dest.addr[i] = 0xFF;//broadcast
    header->dhcp_hdr.client_hardware_address[i] = ETHInstance.Init.MACAddr[i];
    header->dhcp_hdr.options[i + 6] = ETHInstance.Init.MACAddr[i];
  }
  header->eth_hdr.type = ETH_PROTOCOL_IPV4;
  header->ipv4_hdr.versionAndHeaderLength = 0x45;
  header->ipv4_hdr.serviceType = 0;
  header->ipv4_hdr.totalLength = ETH_Swap(ETH_IPV4_HEADER_LENGTH + ETH_UDP_HEADER_LENGTH + sizeof(ETH_DHCP_Header) + hostnameLength + 2);
  header->ipv4_hdr.identification = 0;
  header->ipv4_hdr.flagsAndOffset = 0;
  header->ipv4_hdr.ttl = 5;
  header->ipv4_hdr.protocol = ETH_IP_PROTOCOL_UDP;
  header->ipv4_hdr.headerChecksum = 0;
  header->ipv4_hdr.sourceIP = 0;
  header->ipv4_hdr.destIP = BROADCAST_IP;
  header->udp_hdr.source_port = 0x4400; // 68
  header->udp_hdr.dest_port = 0x4300; // 67
  header->udp_hdr.length = ETH_Swap(ETH_UDP_HEADER_LENGTH + sizeof(ETH_DHCP_Header) + hostnameLength + 2);
  header->udp_hdr.checksum = 0; // disabled

  header->dhcp_hdr.opcode = 1; //request
  header->dhcp_hdr.hwtype = 1; //Ethernet
  header->dhcp_hdr.hw_address_length = 6;
  //header->dhcp_hdr.hop_count = 0;
  //header->dhcp_hdr.transaction_id = 0x12345678;
  header->dhcp_hdr.number_of_seconds = 0x200; // 2 seconds
  header->dhcp_hdr.flags = 0x80; // Expected broadcast reply
  header->dhcp_hdr.magic[0] = 0x63;
  header->dhcp_hdr.magic[1] = 0x82;
  header->dhcp_hdr.magic[2] = 0x53;
  header->dhcp_hdr.magic[3] = 0x63;

  header->dhcp_hdr.options[0] = 0x35; // DHCP Message type
  header->dhcp_hdr.options[1] = 1; // length
  header->dhcp_hdr.options[2] = 1; // DHCP Discover

  header->dhcp_hdr.options[3] = 0x3d; // Client identifier
  header->dhcp_hdr.options[4] = 7; // length
  header->dhcp_hdr.options[5] = 1; // Ethernet
  // 6-11 - MAC address

  header->dhcp_hdr.options[12] = 0x39; // Maximum DHCP Message size
  header->dhcp_hdr.options[13] = 2; // length
  header->dhcp_hdr.options[14] = 0x05;
  header->dhcp_hdr.options[15] = 0xDC; // 1500

  header->dhcp_hdr.options[16] = 0x0C; // Host name
  header->dhcp_hdr.options[17] = hostnameLength; // length
  strcpy(&header->dhcp_hdr.options[18], hostName);

  header->dhcp_hdr.options[18 + hostnameLength] = 0xFF; // End

  queue_add(&eth_user_queue, sizeof(DHCP_Header) + hostnameLength + 2);
}
