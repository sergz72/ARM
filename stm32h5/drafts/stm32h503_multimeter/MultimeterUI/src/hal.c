#include "board.h"
#include <lcd_sh1107.h>
#include <string.h>
#include "multimeter.h"

#define SIZE (LCD_HEIGHT * LCD_WIDTH / 8)

extern int long_delay;
extern unsigned int long_press;
extern unsigned int key;

static unsigned char lcd_buffer[SIZE], *lcd_buffer_p = lcd_buffer;

volatile int capacity_measurement_done;

multimeter_result_t multimeter_result_hal =
{
  .frequency_hz = 0,
  .inductance_nH = 0,
  .diode_voltage_uV = {0,0},
  .voltage_current = {{0, 0}, {0, 0}},
  .resistance_mOhm = {0, 0},
  .temperature_Cx100 = 0,
  .vdda_mV = 0,
  .capacitance = {0, 0, 0}
};

unsigned int measurement_statuses = 0;

void discharge_off(void)
{
}

void discharge_on(void)
{
}

void charge_off(void)
{
}

void charge_on(int channel)
{
}

unsigned int get_capacity_measurement_start_time(void)
{
  return 0;
}

unsigned int get_capacity_measurement_end_time(void)
{
  return 0;
}

void capacity_measurement_start(int channel)
{
  discharge_off();
  charge_on(channel);
}

void SH1107_Write(int num_bytes, unsigned char dc, const unsigned char *buffer)
{
  if (dc)
  {
    memcpy(lcd_buffer_p, buffer, num_bytes);
    lcd_buffer_p += num_bytes;
    if (lcd_buffer_p - lcd_buffer >= SIZE)
      lcd_buffer_p = lcd_buffer;
  }
}

void delayms(unsigned int ms)
{

}

void delay(unsigned int us)
{

}

int get_lcd_buffer_bit(int x, int y)
{
  unsigned char *p = lcd_buffer + (x >> 3U) + y * LCD_WIDTH/8;
  int bit = x & 7;
  return *p & (1 << bit);
}

unsigned int get_keyboard_status(void)
{
  if (long_delay)
  {
    long_delay--;
    if (!long_delay)
    {
      unsigned int state = long_press ? long_press : key;
      long_press = 0;
      return state;
    }
  }
  return 0;
}

unsigned int check_measurements_statuses(void)
{
  return measurement_statuses;
}

void start_voltage_measurements(void)
{
  measurement_statuses |= VOLTAGE1_MEASUREMENT | VOLTAGE2_MEASUREMENT;
}

void start_frequency_measurement(void)
{
  measurement_statuses |= FREQUENCY_MEASUREMENT;
}

void start_resistance_measurement(int channel, enum resistance_measurements_modes mode)
{
  measurement_statuses |= channel ? RESISTANCE2_MEASUREMENT_HIGH : RESISTANCE1_MEASUREMENT_HIGH;
}

void start_capacitance_measurement(int channel)
{
  measurement_statuses |= CAPACITANCE_MEASUREMENT_1K;
}

void start_current_measurement(int channel)
{
  measurement_statuses |= channel ? CURRENT2_MEASUREMENT : CURRENT1_MEASUREMENT;
}

unsigned int start_extra_measurements(int channel, int extra_measurement_no)
{
  measurement_statuses |= channel ? VDDA_MEASUREMENT : TEMPERATURE_MEASUREMENT;
  return channel ? VDDA_MEASUREMENT : TEMPERATURE_MEASUREMENT;
}

void start_diode_voltage_measurement(int channel)
{
  measurement_statuses |= channel ? DIODE_VOLTAGE_MEASUREMENT2 : DIODE_VOLTAGE_MEASUREMENT1;
}

unsigned int finish_voltage_measurement(int channel)
{
  measurement_statuses &= ~(channel ? VOLTAGE2_MEASUREMENT : VOLTAGE1_MEASUREMENT);
  return multimeter_result_hal.voltage_current[channel].voltage_uV;
}

unsigned int finish_frequency_measurement(void)
{
  measurement_statuses &= ~FREQUENCY_MEASUREMENT;
  return multimeter_result_hal.frequency_hz;
}

unsigned int finish_resistance_measurement(int channel, int mode)
{
  measurement_statuses &= ~(channel ? RESISTANCE2_MEASUREMENT_HIGH : RESISTANCE1_MEASUREMENT_HIGH);
  return mode ? multimeter_result_hal.resistance_mOhm[1] : multimeter_result_hal.resistance_mOhm[0];
}

unsigned int finish_current_measurement(int channel)
{
  measurement_statuses &= ~(channel ? CURRENT2_MEASUREMENT : CURRENT1_MEASUREMENT);
  return multimeter_result_hal.voltage_current[channel].current_nA;
}

void finish_capacitance_measurement(void)
{
  measurement_statuses &= ~CAPACITANCE_MEASUREMENT_100K;
  multimeter_result.capacitance = multimeter_result_hal.capacitance;
}

unsigned int finish_capacitance_measurement_1k(void)
{
  measurement_statuses &= ~CAPACITANCE_MEASUREMENT_1K;
  multimeter_result.capacitance = multimeter_result_hal.capacitance;
  return 0;
}

unsigned int finish_temperature_measurement(void)
{
  measurement_statuses &= ~TEMPERATURE_MEASUREMENT;
  return multimeter_result_hal.temperature_Cx100;
}

unsigned int finish_vdda_measurement(void)
{
  measurement_statuses &= ~VDDA_MEASUREMENT;
  return multimeter_result_hal.vdda_mV;
}

unsigned int finish_diode_voltage_measurement(int channel)
{
  measurement_statuses &= ~(channel ? DIODE_VOLTAGE_MEASUREMENT2 : DIODE_VOLTAGE_MEASUREMENT1);
  return multimeter_result_hal.diode_voltage_uV[channel];
}

void power_off(void)
{

}