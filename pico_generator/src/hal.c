#include <hardware/clocks.h>
#include <pico/printf.h>
#include "board.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "ui.h"

unsigned int lcd_slice_num, pwm_slice_num[2], pwm_channel[2];

void ST7735_WriteBytes(unsigned char *data, unsigned int size)
{
  spi_write_blocking(ST7735_SPI_PORT, data, size);
}

void ST7735_SET_PWM(unsigned int duty)
{
  pwm_set_chan_level(lcd_slice_num, PWM_CHAN_B, duty);
}

void delayms(unsigned int ms)
{
  sleep_ms(ms);
}

void enableOutput(unsigned int mode, unsigned int channel, unsigned int enabled)
{
  if (mode == MODE_PWM)
    pwm_set_enabled(pwm_slice_num[channel], enabled ? true : false);
}

void setParams(unsigned int channel, unsigned int mode, unsigned int frequency, unsigned int duty, unsigned int amplitude)
{
  unsigned int hz, div, wrap;

  if (mode == MODE_PWM && frequency != 0 && duty <= 10000)
  {
    hz = clock_get_hz(clk_sys) * 10;
    div = ((hz / frequency) >> 16) + 1;
    if (div <= 255)
    {
      wrap = hz / div / frequency;
      duty = duty * wrap / 10000;
      printf("hz=%u div=%u wrap=%u duty=%u\n", hz, div, wrap, duty);
      pwm_set_clkdiv(pwm_slice_num[channel], (float) div);
      pwm_set_wrap(pwm_slice_num[channel], wrap);
      pwm_set_chan_level(pwm_slice_num[channel], pwm_channel[channel], duty);
    }
  }
}

void SystemInit(void)
{
  spi_init(ST7735_SPI_PORT, 10000 * 1000);
  gpio_set_function(ST7735_CLK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(ST7735_MOSI_PIN, GPIO_FUNC_SPI);

  gpio_init(ST7735_RST_PIN);
  gpio_set_dir(ST7735_RST_PIN, GPIO_OUT);
  gpio_init(ST7735_DC_PIN);
  gpio_set_dir(ST7735_DC_PIN, GPIO_OUT);
  gpio_init(ST7735_CS_PIN);
  gpio_set_dir(ST7735_CS_PIN, GPIO_OUT);
  gpio_init(ST7735_BL_PIN);
  gpio_set_dir(ST7735_BL_PIN, GPIO_OUT);

  gpio_put(ST7735_CS_PIN, 1);
  gpio_put(ST7735_DC_PIN, 0);
  gpio_put(ST7735_BL_PIN, 1);

  // LCD PWM Config
  gpio_set_function(ST7735_BL_PIN, GPIO_FUNC_PWM);
  lcd_slice_num = pwm_gpio_to_slice_num(ST7735_BL_PIN);
  pwm_set_wrap(lcd_slice_num, 100);
  pwm_set_chan_level(lcd_slice_num, PWM_CHAN_B, 1);
  pwm_set_clkdiv(lcd_slice_num,50);
  pwm_set_enabled(lcd_slice_num, true);

  // PWM Config
  gpio_set_function(PWM1_PIN, GPIO_FUNC_PWM);
  gpio_set_function(PWM2_PIN, GPIO_FUNC_PWM);
  pwm_slice_num[0] = pwm_gpio_to_slice_num(PWM1_PIN);
  pwm_slice_num[1] = pwm_gpio_to_slice_num(PWM2_PIN);
  pwm_channel[0] = pwm_gpio_to_channel(PWM1_PIN);
  pwm_channel[1] = pwm_gpio_to_channel(PWM2_PIN);
}
