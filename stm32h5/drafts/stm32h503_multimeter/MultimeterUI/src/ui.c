#include "board.h"
#include "ui.h"
#include <lcd_sh1107.h>
#include <limits.h>
#include <fonts/font10.h>
#include <multimeter.h>
#include <stdlib.h>

#define MAIN_FONT courierNew10ptFontInfo
#define VALUE_X (2*(MAIN_FONT.character_max_width+MAIN_FONT.character_spacing))
#define VDDA_X  (7*(MAIN_FONT.character_max_width+MAIN_FONT.character_spacing))
#define F_X     (MAIN_FONT.character_max_width+MAIN_FONT.character_spacing)

static void DrawF(void)
{
  LcdDrawText(0, MAIN_FONT.char_height*7, "F  0.000000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  multimeter_result.frequency_hz = 0;
}

static void DrawEmptyLine(void)
{
  LcdDrawText(0, MAIN_FONT.char_height*8, "           ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
}

static void DrawMode(void)
{
  switch (multimeter_mode)
  {
    case FREQUENCY:
      DrawF();
      DrawEmptyLine();
      LcdDrawText(0, MAIN_FONT.char_height*9, "F| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, NULL);
      break;
    case RESISTANCE:
    case CONTINUITY:
      LcdDrawText(0, MAIN_FONT.char_height*7, "1R---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
      LcdDrawText(0, MAIN_FONT.char_height*8, "2R---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
      multimeter_result.resistance_mOhm[0] = UINT_MAX;
      multimeter_result.resistance_mOhm[1] = UINT_MAX;
      if (multimeter_mode == RESISTANCE)
        LcdDrawText(0, MAIN_FONT.char_height*9, "R| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, NULL);
      else
        LcdDrawText(0, MAIN_FONT.char_height*9, "I| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, NULL);
      break;
    case DIODE_TEST:
      LcdDrawText(0, MAIN_FONT.char_height*7, "1D---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
      LcdDrawText(0, MAIN_FONT.char_height*8, "2D---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
      multimeter_result.diode_voltage_uV[0] = UINT_MAX;
      multimeter_result.diode_voltage_uV[1] = UINT_MAX;
      LcdDrawText(0, MAIN_FONT.char_height*9, "D| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, NULL);
      break;
    case CAPACITANCE:
      LcdDrawText(0, MAIN_FONT.char_height*7, "C   0.000nF", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
      multimeter_result.capacitance.pF = 0;
      DrawEmptyLine();
      LcdDrawText(0, MAIN_FONT.char_height*9, "C| Z| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, NULL);
      break;
    case INDUCTANCE:
      DrawF();
      LcdDrawText(0, MAIN_FONT.char_height*8, "L   0.000uH", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
      multimeter_result.inductance_nH = 0;
      LcdDrawText(0, MAIN_FONT.char_height*9, "L| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, NULL);
  }
}

void UI_Init(void)
{
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  int y = 0;

  LcdDrawText(0, y, "00.0C 0.00V", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);

  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1U- 0.00000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1I-  0.0000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1P 0.000000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);

  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2U- 0.00000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2I-  0.0000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2P 0.000000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);

  DrawMode();

  LcdUpdate();
}

void DrawVoltage(int line, int value_uV)
{
  if (value_uV == INT_MAX)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "OVERVOLT ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  else if (value_uV == INT_MIN)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "UNDERVOLT", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  else
    LcdPrintf("%3d.%05d", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_uV / 1000000, abs(value_uV/10) % 100000);
}

void DrawCurrent(int line, int value_nA)
{
  if (value_nA == INT_MAX)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "OVERCURR ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  else if (value_nA == INT_MIN)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "UNDERCURR", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  LcdPrintf("%3d.%04d", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
            value_nA / 1000000, abs(value_nA/100) % 10000);
}

void DrawPower(int line, int uV, int nA)
{
  if (uV == INT_MAX || uV == INT_MIN || nA == INT_MAX || nA == INT_MIN)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "OVERLOAD ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  else
  {
    if (uV < 0)
      uV = -uV;
    if (nA < 0)
      nA = -nA;
    int uwatt = (int)((long long int)uV * (long long int)nA / 1000000000LL);
    LcdPrintf("%2d.%06d", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              uwatt / 1000000, abs(uwatt) % 1000000);
  }
}

void DrawTemperature(unsigned int value)
{
  if (value > 999)
    value = 999;
  LcdPrintf("%2d.%d", 0, 0, &MAIN_FONT, 0, value / 10, value % 10);
}

void DrawVdda(unsigned int value_mV)
{
  if (value_mV > 9999)
    value_mV = 9999;
  LcdPrintf("%d.%02d", 0, VDDA_X, &MAIN_FONT, 0, value_mV / 1000, (value_mV/10) % 100);
}

void DrawFrequency(int line, unsigned int value)
{
  if (value > 999999999)
    value = 999999999;
  LcdPrintf("%3d.%06d", line*MAIN_FONT.char_height, F_X, &MAIN_FONT, 0, value / 1000000, value % 1000000);
}

void DrawInductance(int line, unsigned int value_nH)
{
  if (value_nH < 1000000)
    LcdPrintf("%3d.%03dn", line*MAIN_FONT.char_height, VALUE_X, &MAIN_FONT, 0,
              value_nH / 1000, value_nH % 1000);
  else
    LcdPrintf("%3d.%03dm", line*MAIN_FONT.char_height, VALUE_X, &MAIN_FONT, 0,
              value_nH / 1000000, (value_nH/1000) % 1000);
}

void DrawCapacitance(int line, unsigned int value_pF)
{
  if (value_pF < 10000000)
    LcdPrintf("%4d.%03dp", line*MAIN_FONT.char_height, F_X, &MAIN_FONT, 0,
              value_pF / 1000, value_pF % 1000);
  else
    LcdPrintf("%4d.%03dn", line*MAIN_FONT.char_height, F_X, &MAIN_FONT, 0,
              value_pF / 1000000, (value_pF/1000) % 1000);
}

void DrawResistance(int line, unsigned int value_mOhm)
{
  if (value_mOhm == UINT_MAX)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, NULL);
  else if (value_mOhm < 10000000)
    LcdPrintf("%4d.%03d ", line*MAIN_FONT.char_height, VALUE_X, &MAIN_FONT, 0,
              value_mOhm / 1000, value_mOhm % 1000);
  else
    LcdPrintf("%4d.%03dK", line*MAIN_FONT.char_height, VALUE_X, &MAIN_FONT, 0,
              value_mOhm / 1000000, (value_mOhm/1000) % 1000);
}

void Process_Timer_Event(unsigned char keyboard_status, unsigned int multimeter_changes)
{
  if (multimeter_changes & TEMPERATURE_CHANGED)
    DrawTemperature(multimeter_result.temperature_Cx100);
  if (multimeter_changes & VDDA_CHANGED)
    DrawVdda(multimeter_result.vdda_mV);
  if (multimeter_changes & VOLTAGE1_CHANGED)
    DrawVoltage(1, multimeter_result.voltage_current[0].voltage_uV);
  if (multimeter_changes & CURRENT1_CHANGED)
    DrawCurrent(2, multimeter_result.voltage_current[0].current_nA);
  if (multimeter_changes & (VOLTAGE1_CHANGED|CURRENT1_CHANGED))
    DrawPower(3, multimeter_result.voltage_current[0].voltage_uV, multimeter_result.voltage_current[0].current_nA);
  if (multimeter_changes & VOLTAGE2_CHANGED)
    DrawVoltage(4, multimeter_result.voltage_current[1].voltage_uV);
  if (multimeter_changes & CURRENT2_CHANGED)
    DrawCurrent(5, multimeter_result.voltage_current[1].current_nA);
  if (multimeter_changes & (VOLTAGE2_CHANGED|CURRENT2_CHANGED))
    DrawPower(6, multimeter_result.voltage_current[1].voltage_uV, multimeter_result.voltage_current[1].current_nA);
  if (multimeter_changes & FREQUENCY_CHANGED && (multimeter_mode == FREQUENCY || multimeter_mode == INDUCTANCE))
    DrawFrequency(7, multimeter_result.frequency_hz);
  if (multimeter_changes & INDUCTANCE_CHANGED && multimeter_mode == INDUCTANCE)
    DrawInductance(8, multimeter_result.frequency_hz);
  if (multimeter_changes & CAPACITANCE_CHANGED && multimeter_mode == CAPACITANCE)
    DrawCapacitance(7, multimeter_result.capacitance.pF);
  if (multimeter_changes & RESISTANCE1_CHANGED && (multimeter_mode == RESISTANCE || multimeter_mode == CONTINUITY))
    DrawResistance(7, multimeter_result.resistance_mOhm[0]);
  if (multimeter_changes & RESISTANCE2_CHANGED && (multimeter_mode == RESISTANCE || multimeter_mode == CONTINUITY))
    DrawResistance(8, multimeter_result.resistance_mOhm[1]);
  if (multimeter_changes & DIODE_VOLTAGE1_CHANGED && multimeter_mode == DIODE_TEST)
    DrawVoltage(7, (int)multimeter_result.diode_voltage_uV[0]);
  if (multimeter_changes & DIODE_VOLTAGE2_CHANGED && multimeter_mode == DIODE_TEST)
    DrawVoltage(8, (int)multimeter_result.diode_voltage_uV[1]);

  switch (keyboard_status)
  {
    case KB_MODE:
      if (multimeter_mode == MULTIMETER_MAX_MODE)
        multimeter_set_mode(MULTIMETER_MIN_MODE);
      else
        multimeter_set_mode(multimeter_mode + 1);
      DrawMode();
      multimeter_changes = 1;
      break;
    case KB_MODE_LONG:
      power_off();
      break;
  }

  if (multimeter_changes)
    LcdUpdate();
}
