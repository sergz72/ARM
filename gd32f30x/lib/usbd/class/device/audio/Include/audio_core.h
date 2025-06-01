/*!
    \file    audio_core.h
    \brief   the header file of USB audio device class core functions

   \version 2024-12-20, V3.0.1, firmware for GD32F30x
*/

/*
    Copyright (c) 2024, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef AUDIO_CORE_H
#define AUDIO_CORE_H

#include "usbd_enum.h"

#define FORMAT_24BIT(x)                              (uint8_t)(x);(uint8_t)((x) >> 8);(uint8_t)((x) >> 16)

/* number of sub-packets in the audio transfer buffer. user can modify this value but always make sure
   that it is an even number and higher than 3 */
#define OUT_PACKET_NUM                               120U

/* total size of the audio transfer buffer */
#define OUT_BUF_MARGIN                               0U
#define TOTAL_OUT_BUF_SIZE                           ((uint32_t)((SPEAKER_OUT_PACKET + OUT_BUF_MARGIN) * OUT_PACKET_NUM))

/* audio configuration descriptor length and interface descriptor size */
#define AD_CONFIG_DESC_SET_LEN                    (sizeof(usb_desc_config_set))
#define AD_INTERFACE_DESC_SIZE                    9U

/* audio standard endpoint and streaming endpoint descriptor size */
#define USB_AD_DESC_SIZ                           0x09U                                    /*!< audio descriptor size */
#define AD_STANDARD_EP_DESC_SIZE                  0x09U                                    /*!< audio standard endpoint descriptor size */
#define AD_STREAMING_EP_DESC_SIZE                 0x07U                                    /*!< audio streaming endpoint descriptor size */

/* audio interface class code */
#define USB_CLASS_AUDIO                           0x01U

/* audio interface subclass codes */
#define AD_SUBCLASS_CONTROL                       0x01U                                    /*!< audio interface control */
#define AD_SUBCLASS_AUDIOSTREAMING                0x02U                                    /*!< audio interface audiostreaming */
#define AD_SUBCLASS_MIDISTREAMING                 0x03U                                    /*!< audio interface midistreaming */

/* audio interface protocol codes */
#define AD_PROTOCOL_UNDEFINED                     0x00U                                    /*!< audio interface undefined */
#define AD_STREAMING_GENERAL                      0x01U                                    /*!< audio interface streaming general */
#define AD_STREAMING_FORMAT_TYPE                  0x02U                                    /*!< audio interface streaming format type */

/* audio class-specific descriptor types */
#define AD_DESCTYPE_UNDEFINED                     0x20U                                    /*!< audio class-specific descriptor undefined */
#define AD_DESCTYPE_DEVICE                        0x21U                                    /*!< audio device descriptor */
#define AD_DESCTYPE_CONFIGURATION                 0x22U                                    /*!< audio configuration descriptor */
#define AD_DESCTYPE_STRING                        0x23U                                    /*!< audio string descriptor */
#define AD_DESCTYPE_INTERFACE                     0x24U                                    /*!< audio interface descriptor */
#define AD_DESCTYPE_ENDPOINT                      0x25U                                    /*!< audio endpoint descriptor */

/* audio control interface descriptor subtypes */
#define AD_CONTROL_HEADER                         0x01U                                    /*!< audio control interface header descriptor */
#define AD_CONTROL_INPUT_TERMINAL                 0x02U                                    /*!< audio control interface input terminal descriptor */
#define AD_CONTROL_OUTPUT_TERMINAL                0x03U                                    /*!< audio control interface output terminal descriptor */
#define AD_CONTROL_MIXER_UNIT                     0x04U                                    /*!< audio control interface maximum unit descriptor */
#define AD_CONTROL_SELECTOR_UNIT                  0x05U                                    /*!< audio control interface selector unit descriptor */
#define AD_CONTROL_FEATURE_UNIT                   0x06U                                    /*!< audio control interface feature unit descriptor */
#define AD_CONTROL_PROCESSING_UNIT                0x07U                                    /*!< audio control interface processing unit descriptor */
#define AD_CONTROL_EXTENSION_UNIT                 0x08U                                    /*!< audio control interface extension unit descriptor */

/* audio input/output terminal and streaming interface descriptor size */
#define AD_INPUT_TERMINAL_DESC_SIZE               0x0CU                                    /*!< audio input terminal interface descriptor size */
#define AD_OUTPUT_TERMINAL_DESC_SIZE              0x09U                                    /*!< audio output terminal interface descriptor size */
#define AD_STREAMING_INTERFACE_DESC_SIZE          0x07U                                    /*!< audio streaming interface descriptor size */

/* audio control types */
#define AD_CONTROL_MUTE                           0x01U                                    /*!< audio control mute type */
#define AD_CONTROL_VOLUME                         0x02U                                    /*!< audio control volume type */

/* audio format types */
#define AD_FORMAT_TYPE_I                          0x01U                                    /*!< audio format typeI */
#define AD_FORMAT_TYPE_III                        0x03U                                    /*!< audio format typeIII */

/* endpoint types */
#define USB_ENDPOINT_TYPE_ISOCHRONOUS             0x01U                                    /*!< audio isochronous endpoint type */
#define AD_ENDPOINT_GENERAL                       0x01U                                    /*!< audio general endpoint type */

/* audio request types */
#define AD_REQ_UNDEFINED                          0x00U                                    /*!< audio undefined request */
#define AD_REQ_SET_CUR                            0x01U                                    /*!< current setting attribute request */
#define AD_REQ_GET_CUR                            0x81U                                    /*!< current getting attribute request */
#define AD_REQ_SET_MIN                            0x02U                                    /*!< setting minimum range attribute request */
#define AD_REQ_GET_MIN                            0x82U                                    /*!< getting minimum range attribute request */
#define AD_REQ_SET_MAX                            0x03U                                    /*!< setting maximum range attribute request */
#define AD_REQ_GET_MAX                            0x83U                                    /*!< getting maximum range attribute request */
#define AD_REQ_SET_RES                            0x04U                                    /*!< setting range attribute request */
#define AD_REQ_GET_RES                            0x84U                                    /*!< getting range attribute request */
#define AD_REQ_SET_MEM                            0x05U                                    /*!< setting memory attribute request */
#define AD_REQ_GET_MEM                            0x85U                                    /*!< getting memory attribute request */
#define AD_REQ_GET_STAT                           0xFFU                                    /*!< getting state request */

/* streaming control types */
#define AD_OUT_STREAMING_CTRL                     0x02U                                    /*!< audio streaming control OUT */
#define AD_IN_STREAMING_CTRL                      0x05U                                    /*!< audio streaming control IN */
 
/* audio stream interface number */
enum {
    SPEAK_INTERFACE_COUNT,                                                                /*!< audio speaker interface count */
    CONFIG_DESC_AS_ITF_COUNT                                                              /*!< audio system interface count */
};

#define AC_ITF_TOTAL_LEN                             (sizeof(usb_desc_AC_itf) + CONFIG_DESC_AS_ITF_COUNT*(sizeof(usb_desc_input_terminal) + \
                                                      sizeof(usb_desc_mono_feature_unit) + sizeof(usb_desc_output_terminal)))

#pragma pack(1)

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< header descriptor subtype */
    uint16_t bcdADC;                  /*!< audio device class specification release number in binary-coded decimal */
    uint16_t wTotalLength;            /*!< total number of bytes */
    uint8_t  bInCollection;           /*!< the number of the streaming interfaces */
    uint8_t  baInterfaceNr;           /*!< interface number of the streaming interfaces */
} usb_desc_AC_itf;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< general audio system descriptor subtype */
    uint8_t  bTerminalLink;           /*!< the terminal ID */
    uint8_t  bDelay;                  /*!< delay introduced by the data path */
    uint16_t wFormatTag;              /*!< the audio data format */
} usb_desc_AS_itf;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< input terminal descriptor subtype. */
    uint8_t  bTerminalID;             /*!< constant uniquely identifying the terminal within the audio function */
    uint16_t wTerminalType;           /*!< constant characterizing the type of terminal */
    uint8_t  bAssocTerminal;          /*!< ID of the output terminal */
    uint8_t  bNrChannels;             /*!< number of logical output channels */
    uint16_t wChannelConfig;          /*!< describes the spatial location of the logical channels */
    uint8_t  iChannelNames;           /*!< index of a channel string descriptor */
    uint8_t  iTerminal;               /*!< index of a string descriptor */
} usb_desc_input_terminal;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< output terminal descriptor subtype */
    uint8_t  bTerminalID;             /*!< constant uniquely identifying the terminal within the audio function */
    uint16_t wTerminalType;           /*!< constant characterizing the type of terminal */
    uint8_t  bAssocTerminal;          /*!< constant, identifying the input terminal to which this output terminal is associated */
    uint8_t  bSourceID;               /*!< ID of the unit or terminal */
    uint8_t  iTerminal;               /*!< index of a string descriptor */
} usb_desc_output_terminal;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< feature unit descriptor subtype */
    uint8_t  bUnitID;                 /*!< constant uniquely identifying the unit within the audio function */
    uint8_t  bSourceID;               /*!< ID of the unit or terminal */
    uint8_t  bControlSize;            /*!< size in bytes of an element of the bmaControls() array */
    uint8_t  bmaControls0;            /*!< a bit set to 1 indicates that the mentioned control is supported for master channel 0 */
    uint8_t  bmaControls1;            /*!< a bit set to 1 indicates that the mentioned control is supported for logical channel 1 */
    uint8_t  iFeature;                /*!< index of a string descriptor */
} usb_desc_mono_feature_unit;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< feature unit descriptor subtype */
    uint8_t  bUnitID;                 /*!< constant uniquely identifying the unit within the audio function */
    uint8_t  bSourceID;               /*!< ID of the unit or terminal */
    uint8_t  bControlSize;            /*!< size in bytes of an element of the bmaControls() array */
    uint16_t bmaControls0;            /*!< a bit set to 1 indicates that the mentioned control is supported for master channel 0 */
    uint16_t bmaControls1;            /*!< a bit set to 1 indicates that the mentioned control is supported for logical channel 1 */
    uint16_t bmaControls2;            /*!< a bit set to 1 indicates that the mentioned control is supported for logical channel 2 */
    uint8_t  iFeature;                /*!< index of a string descriptor */
} usb_desc_stereo_feature_unit;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< format type descriptor subtype */
    uint8_t  bFormatType;             /*!< constant identifying the format type */
    uint8_t  bNrChannels;             /*!< indicates the number of physical channels in the audio data stream */
    uint8_t  bSubFrameSize;           /*!< the number of bytes occupied by one audio subframe */
    uint8_t  bBitResolution;          /*!< the number of effectively used bits from the available bits in an audio subframe */
    uint8_t  bSamFreqType;            /*!< indicates how the sampling frequency can be programmed */
    uint8_t  bSamFreq[3];             /*!< sampling frequency ns in Hz for this isochronous data endpoint */
} usb_desc_format_type;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bEndpointAddress;        /*!< the address of the endpoint */
    uint8_t  bmAttributes;            /*!< transfer type and synchronization type */
    uint16_t wMaxPacketSize;          /*!< maximum packet size this endpoint is capable of sending or receiving */
    uint8_t  bInterval;               /*!< left to the designer's discretion */
    uint8_t  bRefresh;                /*!< reset to 0 */
    uint8_t  bSynchAddress;           /*!< reset to 0 */
} usb_desc_std_ep;

typedef struct {
    usb_desc_header header;           /*!< descriptor header, including type and size */
    uint8_t  bDescriptorSubtype;      /*!< general endpoint descriptor subtype */
    uint8_t  bmAttributes;            /*!< transfer type and synchronization type */
    uint8_t  bLockDelayUnits;         /*!< indicates the units used for the lock delay field */
    uint16_t wLockDelay;              /*!< indicates the time it takes this endpoint to reliably lock its internal clock recovery circuitry */
} usb_desc_AS_ep;

#pragma pack()

/* USB configuration descriptor structure */
typedef struct {
    usb_desc_config             config;                            /*!< configuration descriptor */
    usb_desc_itf                std_itf;                           /*!< interface descriptor */
    usb_desc_AC_itf             ac_itf;                            /*!< auido controller interface descriptor */
    usb_desc_input_terminal     in_terminal;                       /*!< input terminal descriptor */
    usb_desc_stereo_feature_unit  feature_unit;                      /*!< feature unit descriptor */
    usb_desc_output_terminal    out_terminal;                      /*!< output terminal descriptor */
    usb_desc_itf                std_as_itf_zeroband;               /*!< zeroband configuration standard audio streaming interface descriptor */
    usb_desc_itf                std_as_itf_opera;                  /*!< standard audio streaming interface descriptor */
    usb_desc_AS_itf             as_itf;                            /*!< audio correlation descriptor */
    usb_desc_format_type        format_typeI;                      /*!< typeI format type descriptor */
    usb_desc_std_ep             std_endpoint;                      /*!< standard endpoint descriptor */
    usb_desc_AS_ep              as_endpoint;                       /*!< audio dependent isochronous data endpoint descriptor */
} usb_desc_config_set;

typedef struct {
    /* main buffer for audio data out transfers and its relative pointers */
    uint8_t  isoc_out_buff[TOTAL_OUT_BUF_SIZE * 2];                /*!< audio isochronous out data buff */
    uint8_t* isoc_out_wrptr;                                       /*!< audio isochronous out data write pointer */
    uint8_t* isoc_out_rdptr;                                       /*!< audio isochronous out data read pointer */
    uint16_t buf_free_size;                                        /*!< audio data buff free size */
    uint16_t dam_tx_len;                                           /*!< audio amplifier transmit length */

    /* usb receive buffer */
    uint8_t usb_rx_buffer[SPEAKER_OUT_MAX_PACKET];                 /*!< audio receive buffer */

    /* main buffer for audio control requests transfers and its relative variables */
    uint8_t  audioctl[64];                                         /*!< audio control requests transfers buff */
    uint8_t  audioctl_unit;                                        /*!< audio control requests unit */
    uint32_t audioctl_len;                                         /*!< audio control requests length */
    uint16_t audioctl_value;                                       /*!< audio control requests value */
    uint8_t  audioctl_req_type;                                    /*!< audio control requests type */
    uint32_t volume;                                               /*!< audio volume */
    uint32_t mute;                                                 /*!< audio mute */

    uint32_t play_flag;                                            /*!< audio device play flag */
} usbd_audio_handler;

extern usb_desc audio_desc;
extern usb_class audio_class;

#endif /* AUDIO_CORE_H */

