#ifndef _ETH_UDP_H
#define _ETH_UDP_H

typedef struct __attribute__((__packed__))
{
  unsigned short source_port;
  unsigned short dest_port;
  unsigned short length;
  unsigned short checksum;
} ETH_UDP_Header;

#define ETH_UDP_HEADER_LENGTH 8

#include <eth.h>

void ETH_UDP_Handler(unsigned int sourceIP, ETH_UDP_Header *udp_hdr);
void ETH_UDP_BroadcastHandler(unsigned int sourceIP, ETH_UDP_Header *udp_hdr);
void ETH_UDP_Send(unsigned short sourcePort, unsigned int destIP, unsigned short destPort, void *data, unsigned int data_length);
void ETH_DHCP_Send(unsigned char *hostName);

#endif
