/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#include "usbd_core.h"
#include "audio_class.h"
#include "audio_desc.h"
#include "usbd_int.h"
#include "audio_codec.h"
#include "delay_systick.h"
#include "board.h"
#include "tlv.h"

/** @addtogroup AT32F403A_periph_examples
  * @{
  */

/** @addtogroup 403A_USB_device_audio USB_device_audio
  * @{
  */

static usbd_core_type usb_core_dev;
static int led_state;

/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  led_state = 0;
  int cnt = 0;

  /* config nvic priority group */
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /* audio codec init */
  audio_codec_init();
  HALInit(&audio_codec);

  if (tlv_init())
  {
    LED_ON;
    while(1)
      delayms(100);
  }

  // start i2s output
  dma_channel_enable(I2S_DMA_CHANNEL, TRUE);

  /* select usb 48m clcok source */
  /* 168MHz */
  crm_usb_clock_div_set(CRM_USB_DIV_3_5);

  /* enable usb clock */
  crm_periph_clock_enable(CRM_USB_PERIPH_CLOCK, TRUE);

  /* enable usb interrupt */
  nvic_irq_enable(USBFS_L_CAN1_RX0_IRQn, 0, 0);

  /* usb core init */
  usbd_core_init(&usb_core_dev, USB, &audio_class_handler, &audio_desc_handler, 0);

  /* enable usb pull-up */
  usbd_connect(&usb_core_dev);

  while(1)
  {
    audio_codec_loop();
    delayms(100);
    cnt++;
    if (cnt == 10)
    {
      cnt = 0;
      led_state = !led_state;
      if (led_state)
        LED_ON;
      else
        LED_OFF;
    }
  }
}

/**
  * @brief  this function handles usb interrupt.
  * @param  none
  * @retval none
  */
void USBFS_L_CAN1_RX0_IRQHandler(void)
{
  usbd_irq_handler(&usb_core_dev);
}

/**
  * @brief  usb delay millisecond function.
  * @param  ms: number of millisecond delay
  * @retval none
  */
void usb_delay_ms(uint32_t ms)
{
  /* user can define self delay function */
  delayms(ms);
}

/**
  * @brief  usb delay microsecond function.
  * @param  us: number of microsecond delay
  * @retval none
  */
void usb_delay_us(uint32_t us)
{
  delay(us);
}

/**
  * @}
  */

/**
  * @}
  */
