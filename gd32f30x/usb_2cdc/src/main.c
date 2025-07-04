/*!
    \file    main.c
    \brief   USB dual CDC ACM device

    \version 2020-08-01, V3.0.0, firmware for GD32F30x
    \version 2022-06-10, V3.1.0, firmware for GD32F30x
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

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

#include "dual_cdc_acm_core.h"
#include "usbd_hw.h"

usb_dev usbd_dual_cdc;

/*!
    \brief      main routine
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* system clocks configuration */
    rcu_config();

    /* USB device configuration */
    usbd_init(&usbd_dual_cdc, &dual_cdc_desc, &dual_cdc_class);

    /* NVIC configuration */
    nvic_config();

    /* enabled USB pull-up */
    usbd_connect(&usbd_dual_cdc);

    while (USBD_CONFIGURED != usbd_dual_cdc.cur_status) {
        /* wait for standard USB enumeration is finished */
    }

    while (1) {
        if (0U == cdc_check_ready0(&usbd_dual_cdc)) {
            cdc_data_receive0(&usbd_dual_cdc);
        } else {
            cdc_acm_data_send0(&usbd_dual_cdc);
        }

        if (0U == cdc_check_ready1(&usbd_dual_cdc)) {
            cdc_data_receive1(&usbd_dual_cdc);
        } else {
            cdc_acm_data_send1(&usbd_dual_cdc);
        }
    }
}
