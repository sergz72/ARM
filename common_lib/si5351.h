/*
 * si5351.h - Si5351 library for avr-gcc
 *
 * Copyright (C) 2014 Jason Milldrum <milldrum@gmail.com>
 *
 * Many defines derived from clk-si5351.h in the Linux kernel.
 * Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 * Rabeeh Khoury <rabeeh@solid-run.com>
 *
 * do_div() macro derived from /include/asm-generic/div64.h in
 * the Linux kernel.
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SI5351_H_
#define SI5351_H_

#define SI5351_CRYSTAL_LOAD				183
#define  SI5351_CRYSTAL_LOAD_6PF		(1<<6)
#define  SI5351_CRYSTAL_LOAD_8PF		(2<<6)
#define  SI5351_CRYSTAL_LOAD_10PF		(3<<6)

/* Enum definitions */

/*
 * enum si5351_variant - SiLabs Si5351 chip variant
 * @SI5351_VARIANT_A: Si5351A (8 output clocks, XTAL input)
 * @SI5351_VARIANT_A3: Si5351A MSOP10 (3 output clocks, XTAL input)
 * @SI5351_VARIANT_B: Si5351B (8 output clocks, XTAL/VXCO input)
 * @SI5351_VARIANT_C: Si5351C (8 output clocks, XTAL/CLKIN input)
 */
enum si5351_variant {
	SI5351_VARIANT_A = 1,
	SI5351_VARIANT_A3 = 2,
	SI5351_VARIANT_B = 3,
	SI5351_VARIANT_C = 4,
};

enum si5351_clock {SI5351_CLK0, SI5351_CLK1, SI5351_CLK2, SI5351_CLK3,
	                 SI5351_CLK4, SI5351_CLK5, SI5351_CLK6, SI5351_CLK7};

enum si5351_pll {SI5351_PLLA, SI5351_PLLB};

enum si5351_drive {SI5351_DRIVE_2MA, SI5351_DRIVE_4MA, SI5351_DRIVE_6MA, SI5351_DRIVE_8MA};

/* Suggested public function prototypes */

void si5351_init(void);
int si5351_set_freq(int channel, unsigned int, int, enum si5351_clock, unsigned int r_div);
int si5351_set_pll(int channel, unsigned int, enum si5351_pll);
int si5351_clock_enable(int channel, enum si5351_clock, unsigned char);
int si5351_drive_strength(int channel,enum si5351_clock, enum si5351_drive);
int si5351_update_status(int channel);
void si5351_set_correction(int channel, int);

int si5351_write_bulk(int channel, unsigned char addr, unsigned char bytes, unsigned char *data);
int si5351_write(int channel, unsigned char addr, unsigned char data);
int si5351_read(int channel, unsigned char addr, unsigned char *data);

#endif /* SI5351_H_ */
