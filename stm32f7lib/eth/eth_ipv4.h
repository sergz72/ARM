#ifndef _ETH_IPV4_H
#define _ETH_IPV4_H

typedef struct __attribute__((__packed__))
{
  unsigned char versionAndHeaderLength;
  unsigned char serviceType;
  unsigned short totalLength;
  unsigned short identification;
  unsigned short flagsAndOffset;
  unsigned char ttl;
  unsigned char protocol;
  unsigned short headerChecksum;
  unsigned int sourceIP;
  unsigned int destIP;
} ETH_IPV4_Header;

#define ETH_IP_PROTOCOL_ICMP 1
#define ETH_IP_PROTOCOL_UDP 17
#define ETH_IPV4_HEADER_LENGTH 20

#define IP4_ADDR(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))
#define IP4_ADDR1(a) (a & 0xFF)
#define IP4_ADDR2(a) ((a >> 8) & 0xFF)
#define IP4_ADDR3(a) ((a >> 16) & 0xFF)
#define IP4_ADDR4(a) ((a >> 24) & 0xFF)

extern const unsigned int BROADCAST_IP;

#endif
