/*!
    \file    usbd_conf.h
    \brief   usb device driver basic configuration

    \version 2023-06-30, V2.1.6, firmware for GD32F30x
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

#ifndef USBD_CONF_H
#define USBD_CONF_H

#include "gd32f30x.h"
#include "gd32f303c_audio_codec.h"

#define USBD_CFG_MAX_NUM                1U
#define USBD_ITF_MAX_NUM                2U

#define USBD_AD_INTERFACE               0U

/* class layer parameter */
#define AD_TOTAL_IF_NUM                 0x02U

#define AD_OUT_EP                       EP_OUT(1)

/* endpoint0, Rx/Tx buffers address offset */
#define EP0_RX_ADDR                     (0x20U)
#define EP0_TX_ADDR                     (0x60U)

#define AD_BUF_ADDR                     (AD_BUF0_ADDR | (AD_BUF1_ADDR << 16U))

#define AD_BUF0_ADDR                    (0xA0U)
#define AD_BUF1_ADDR                    (0xF0U)

/* speaker parameter */
#define USBD_SPEAKER_FREQ               USBD_AD_FREQ_16K
#define SPEAKER_OUT_BIT_RESOLUTION      16U
#define SPEAKER_OUT_CHANNEL_NBR         2U /* Mono = 1, Stereo = 2 */
#define SPEAKER_OUT_PACKET              (uint32_t)(((USBD_SPEAKER_FREQ * \
                                                    (SPEAKER_OUT_BIT_RESOLUTION / 8U) * \
                                                     SPEAKER_OUT_CHANNEL_NBR) / 1000U))

#define SPEAKER_OUT_MAX_PACKET          SPEAKER_OUT_PACKET

/* Audio frequency in Hz */
#define USBD_AD_FREQ_48K                48000U
#define USBD_AD_FREQ_44K                44100U
#define USBD_AD_FREQ_16K                16000U
#define USBD_AD_FREQ_22K                22000U

#define DEFAULT_VOLUME                  65U    /* Default volume in % (Mute=0%, Max = 100%) in Logarithmic values.
                                                 To get accurate volume variations, it is possible to use a logarithmic
                                                 coversion table to convert from percentage to logarithmic law.
                                                 In order to keep this example code simple, this conversion is not used.*/

/* USB user string supported */
#define USB_SUPPORT_USER_STRING_DESC

/* endpoint count used by the audio device */
#define EP_COUNT                        (2U)

#define USB_STRING_COUNT                4U

/* base address offset of the allocation buffer, used for buffer descriptor table and packet memory */
#define BTABLE_OFFSET                   (0x0000U)

#endif /* USBD_CONF_H */
