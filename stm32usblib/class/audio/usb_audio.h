#ifndef USB_AUDIO_H
#define USB_AUDIO_H

#include <usb.h>

typedef struct
{
  unsigned int data_endpoint_no, is_initialized;
} USB_AUDIO_HandleTypeDef;

#endif