#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <hardware/clocks.h>
#include <pico/stdio.h>
#include "board.h"

static void InputInit(unsigned int pin)
{
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_IN);
  gpio_pull_down(pin);
}

static void LEDInit(void)
{
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

static inline uint bits_packed_per_word(uint pin_count)
{
  const uint SHIFT_REG_WIDTH = 32;
  return SHIFT_REG_WIDTH - (SHIFT_REG_WIDTH % pin_count);
}

static void LogicAnalyserInit(void)
{
  // Load a program to capture n pins. This is just a single `in pins, n`
  // instruction with a wrap.
  uint16_t capture_prog_instr = pio_encode_in(pio_pins, CAPTURE_PIN_COUNT);
  struct pio_program capture_prog = {
      .instructions = &capture_prog_instr,
      .length = 1,
      .origin = -1
  };
  uint offset = pio_add_program(CAPTURE_PIO, &capture_prog);

  // Configure state machine to loop over this `in` instruction forever,
  // with autopush enabled.
  pio_sm_config c = pio_get_default_sm_config();
  sm_config_set_in_pins(&c, CAPTURE_PIN_BASE);
  sm_config_set_wrap(&c, offset, offset);
  sm_config_set_clkdiv(&c, 4); // 48 MHz
  // Note that we may push at a < 32 bit threshold if pin_count does not
  // divide 32. We are using shift-to-right, so the sample data ends up
  // left-justified in the FIFO in this case, with some zeroes at the LSBs.
  sm_config_set_in_shift(&c, true, true, bits_packed_per_word(CAPTURE_PIN_COUNT));
  sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
  pio_sm_init(pio0, CAPTURE_SM, offset, &c);
}

void LogicAnalyserStop(void)
{
  pio_sm_set_enabled(CAPTURE_PIO, CAPTURE_SM, false);
  // Need to clear _input shift counter_, as well as FIFO, because there may be
  // partial ISR contents left over from a previous run. sm_restart does this.
  pio_sm_clear_fifos(CAPTURE_PIO, CAPTURE_SM);
  pio_sm_restart(CAPTURE_PIO, CAPTURE_SM);
}

void LogicAnalyserStart(void)
{
  pio_sm_set_enabled(CAPTURE_PIO, CAPTURE_SM, true);
}

void SystemInit(void)
{
  /*clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                  CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS,
                  144 * MHZ , 144 * MHZ);

  // CLK peri is clocked from clk_sys so need to change clk_peri's freq
  clock_configure(clk_peri,
                  0,
                  CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                  48 * MHZ,
                  48 * MHZ);*/

  set_sys_clock_khz(144000, true);
  stdio_init_all();

  LEDInit();
  InputInit(0);
  InputInit(1);
  LogicAnalyserInit();
  clock_gpio_init(21, CLOCKS_CLK_GPOUT0_CTRL_AUXSRC_VALUE_CLK_SYS, 12);
}
