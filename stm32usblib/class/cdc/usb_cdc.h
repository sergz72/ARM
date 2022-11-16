#ifndef _USB_CDC_H
#define _USB_CDC_H

#include <usb.h>

#define CDC_DATA_FS_MAX_PACKET_SIZE                 64  /* Endpoint IN & OUT Packet size */
#define CDC_CMD_PACKET_SIZE                         8  /* Control Endpoint Packet size */

#define CDC_DATA_FS_IN_PACKET_SIZE                  CDC_DATA_FS_MAX_PACKET_SIZE
#define CDC_DATA_FS_OUT_PACKET_SIZE                 CDC_DATA_FS_MAX_PACKET_SIZE

typedef void serial_receive_proc(unsigned char *Buf, unsigned int Len);

typedef struct
{
  unsigned int data[CDC_DATA_FS_MAX_PACKET_SIZE/4];      /* Force 32bits alignment */
  unsigned int  CmdOpCode;
  unsigned int  CmdLength;
  unsigned char *RxBuffer;
  unsigned char *TxBuffer;
  unsigned int RxLength;
  unsigned int TxLength;

  __IO unsigned int TxState;
  __IO unsigned int RxState;
  serial_receive_proc *serial_receive;
  unsigned int data_endpoint_no, control_endpoint_no, is_initialized;
} USB_CDC_HandleTypeDef;

typedef void CDC_receive(unsigned char *buf, unsigned int len);

unsigned int CDC_Transmit(unsigned int epnum, unsigned char* Buf, unsigned int Len);

extern const USB_ClassTypeDef  CDC_Class;

#endif
