#include "board.h"
#include "at32f403a_407_gpio.h"
#include "sound.h"
#include "tlv.h"

int led_state;

int main(void)
{
    led_state = 0;

    HALInit();

    sound_init();
    if (tlv_init())
    {
        LED_ON;
        while(1)
            delayms(100);
    }

    // start i2s output
    dma_channel_enable(I2S_DMA_CHANNEL, TRUE);

    while(1)
    {
        delayms(1000);
        led_state = !led_state;
        if (led_state)
            LED_ON;
        else
            LED_OFF;
    }
}
