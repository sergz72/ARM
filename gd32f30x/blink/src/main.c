#include "board.h"
#include "gd32f30x_gpio.h"

int led_state;

int main(void)
{
    led_state = 0;

    HALInit();

    while(1)
    {
        delayms(1000);
        led_state = !led_state;
        gpio_bit_write(LED_PORT, LED_PIN, led_state);
    }
}
