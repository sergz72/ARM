#ifndef _ETH_ICMP_H
#define _ETH_ICMP_H

typedef struct __attribute__((__packed__))
{
  unsigned char type;
  unsigned char code;
  unsigned short checksum;
} ETH_ICMP_Header;

#define ETH_ICMP_HEADER_LENGTH 4

#endif
