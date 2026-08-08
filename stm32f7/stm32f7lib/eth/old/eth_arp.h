#ifndef _ETH_ARP_H
#define _ETH_ARP_H

void ETH_UpdateArpTable(unsigned int ip, unsigned char *hwaddr);
unsigned char *ETH_GetHWAddr(unsigned int ip);

#endif
