// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --- //
// cap //
// --- //

#define cap_wrap_target 0
#define cap_wrap 8

static const uint16_t cap_program_instructions[] = {
            //     .wrap_target
    0x80a0, //  0: pull   block                      
    0xa027, //  1: mov    x, osr                     
    0xf801, //  2: set    pins, 1         side 1     
    0x00c5, //  3: jmp    pin, 5                     
    0x0043, //  4: jmp    x--, 3                     
    0xf000, //  5: set    pins, 0         side 0     
    0xa0c1, //  6: mov    isr, x                     
    0x8020, //  7: push   block                      
    0xc001, //  8: irq    nowait 1                   
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program cap_program = {
    .instructions = cap_program_instructions,
    .length = 9,
    .origin = -1,
};

static inline pio_sm_config cap_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + cap_wrap_target, offset + cap_wrap);
    sm_config_set_sideset(&c, 2, true, false);
    return c;
}

static inline void cap_program_init(PIO pio, uint sm, uint offset, uint rhigh_pin, uint in_pin) {
   pio_gpio_init(pio, rhigh_pin);
   pio_gpio_init(pio, in_pin);
   pio_sm_set_consecutive_pindirs(pio, sm, rhigh_pin, 1, true);
   pio_sm_set_consecutive_pindirs(pio, sm, in_pin, 1, false);
   pio_sm_config c = cap_program_get_default_config(offset);
   sm_config_set_set_pins(&c, rhigh_pin, 1);
   sm_config_set_jmp_pin(&c, in_pin);  // for JMP
   sm_config_set_sideset_pins(&c, PICO_DEFAULT_LED_PIN);
   pio_sm_init(pio, sm, offset, &c);
}

#endif

