#include "board.h"
#include "at32f403a_407_gpio.h"

int led_state;

int main(void)
{
    led_state = 0;

    HALInit();

    while(1)
    {
        delayms(1000);
        led_state = !led_state;
        if (led_state)
            LED_PORT->scr = LED_PIN;
        else
            LED_PORT->clr = LED_PIN;

    }
}
