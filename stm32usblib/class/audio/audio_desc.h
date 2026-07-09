#ifndef AUDIO_DESC_H
#define AUDIO_DESC_H

/* USB AUDIO device Configuration Descriptor */
#define AUDIO_INTERFACE_DESC(interface_speaker_no, interface_speaker_as_no, audio_out_endpoint) \
  /* USB Speaker Standard interface descriptor */ \
  0x09,                                 /* bLength */ \
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */ \
  interface_speaker_no,                 /* bInterfaceNumber */ \
  0x00,                                 /* bAlternateSetting */ \
  0x00,                                 /* bNumEndpoints */ \
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */ \
  AUDIO_SUBCLASS_AUDIOCONTROL,          /* bInterfaceSubClass */ \
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */ \
  0x00,                                 /* iInterface */ \
  /* 09 byte*/ \
  /* USB Speaker Class-specific AC Interface Descriptor */ \
  0x09,                                 /* bLength */ \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */ \
  AUDIO_CONTROL_HEADER,                 /* bDescriptorSubtype */ \
  0x00,          /* 1.00 */             /* bcdADC */ \
  0x01, \
  0x27,                                 /* wTotalLength = 39*/ \
  0x00, \
  0x01,                                 /* bInCollection */ \
  0x01,                                 /* baInterfaceNr */ \
  /* 09 byte*/ \
  /* USB Speaker Input Terminal Descriptor */ \
  0x0C,                                 /* bLength */ \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */ \
  AUDIO_CONTROL_INPUT_TERMINAL,         /* bDescriptorSubtype */ \
  0x01,                                 /* bTerminalID */ \
  0x01,                                 /* wTerminalType AUDIO_TERMINAL_USB_STREAMING   0x0101 */ \
  0x01, \
  0x00,                                 /* bAssocTerminal */ \
  0x01,                                 /* bNrChannels */ \
  0x00,                                 /* wChannelConfig 0x0000  Mono */ \
  0x00, \
  0x00,                                 /* iChannelNames */ \
  0x00,                                 /* iTerminal */ \
  /* 12 byte*/ \
  /* USB Speaker Audio Feature Unit Descriptor */ \
  0x09,                                 /* bLength */ \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */ \
  AUDIO_CONTROL_FEATURE_UNIT,           /* bDescriptorSubtype */ \
  AUDIO_OUT_STREAMING_CTRL,             /* bUnitID */ \
  0x01,                                 /* bSourceID */ \
  0x01,                                 /* bControlSize */ \
  AUDIO_CONTROL_MUTE|AUDIO_CONTROL_VOLUME, /* bmaControls(0) */ \
  0,                                    /* bmaControls(1) */ \
  0x00,                                 /* iTerminal */ \
  /* 09 byte*/ \
  /*USB Speaker Output Terminal Descriptor */ \
  0x09,      /* bLength */ \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */ \
  AUDIO_CONTROL_OUTPUT_TERMINAL,        /* bDescriptorSubtype */ \
  0x03,                                 /* bTerminalID */ \
  0x01,                                 /* wTerminalType  0x0301*/ \
  0x03, \
  0x00,                                 /* bAssocTerminal */ \
  0x02,                                 /* bSourceID */ \
  0x00,                                 /* iTerminal */ \
  /* 09 byte*/ \
  /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Zero Bandwith */ \
  /* Interface 1, Alternate Setting 0                                             */ \
  0x09,                                 /* bLength */ \
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */ \
  interface_speaker_as_no,              /* bInterfaceNumber */ \
  0x00,                                 /* bAlternateSetting */ \
  0x00,                                 /* bNumEndpoints */ \
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */ \
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */ \
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */ \
  0x00,                                 /* iInterface */ \
  /* 09 byte*/ \
  /* USB Speaker Standard AS Interface Descriptor - Audio Streaming Operational */ \
  /* Interface 1, Alternate Setting 1                                           */ \
  0x09,                                  /* bLength */ \
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */ \
  interface_speaker_as_no,              /* bInterfaceNumber */ \
  0x01,                                 /* bAlternateSetting */ \
  0x01,                                 /* bNumEndpoints */ \
  USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */ \
  AUDIO_SUBCLASS_AUDIOSTREAMING,        /* bInterfaceSubClass */ \
  AUDIO_PROTOCOL_UNDEFINED,             /* bInterfaceProtocol */ \
  0x00,                                 /* iInterface */ \
  /* 09 byte*/ \
  /* USB Speaker Audio Streaming Interface Descriptor */ \
  0x07,                                 /* bLength */ \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */ \
  AUDIO_STREAMING_GENERAL,              /* bDescriptorSubtype */ \
  0x01,                                 /* bTerminalLink */ \
  0x01,                                 /* bDelay */ \
  0x01,                                 /* wFormatTag AUDIO_FORMAT_PCM  0x0001*/ \
  0x00, \
  /* 07 byte*/ \
  /* USB Speaker Audio Type III Format Interface Descriptor */ \
  0x0B,                                 /* bLength */ \
  AUDIO_INTERFACE_DESCRIPTOR_TYPE,      /* bDescriptorType */ \
  AUDIO_STREAMING_FORMAT_TYPE,          /* bDescriptorSubtype */ \
  AUDIO_FORMAT_TYPE_III,                /* bFormatType */ \
  0x02,                                 /* bNrChannels */ \
  0x02,                                 /* bSubFrameSize :  2 Bytes per frame (16bits) */ \
  16,                                   /* bBitResolution (16-bits per sample) */ \
  0x01,                                 /* bSamFreqType only one frequency supported */ \
  AUDIO_SAMPLE_FREQ(USBD_AUDIO_FREQ),         /* Audio sampling frequency coded on 3 bytes */ \
  /* 11 byte*/ \
  /* Endpoint 1 - Standard Descriptor */ \
  0x09,                                 /* bLength */ \
  USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */ \
  audio_out_endpoint,                   /* bEndpointAddress 1 out endpoint*/ \
  USBD_EP_TYPE_ISOC,                    /* bmAttributes */ \
  AUDIO_PACKET_SZE(USBD_AUDIO_FREQ),    /* wMaxPacketSize in Bytes (Freq(Samples)*2(Stereo)*2(HalfWord)) */ \
  0x01,                                 /* bInterval */ \
  0x00,                                 /* bRefresh */ \
  0x00,                                 /* bSynchAddress */ \
  /* 09 byte*/ \
  /* Endpoint - Audio Streaming Descriptor*/ \
  0x07,                                 /* bLength */ \
  AUDIO_ENDPOINT_DESCRIPTOR_TYPE,       /* bDescriptorType */ \
  AUDIO_ENDPOINT_GENERAL,               /* bDescriptor */ \
  0x00,                                 /* bmAttributes */ \
  0x00,                                 /* bLockDelayUnits */ \
  0x00,                                 /* wLockDelay */ \
  0x00, \
  /* 07 byte*/

#endif