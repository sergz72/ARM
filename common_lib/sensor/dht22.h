#ifndef _DHT22_H
#define _DHT22_H

typedef struct
{
  unsigned short humidity;
  short temperature;
} DHT22_Data;

int DHT22_Read(DHT22_Data *dht_data);

#endif
