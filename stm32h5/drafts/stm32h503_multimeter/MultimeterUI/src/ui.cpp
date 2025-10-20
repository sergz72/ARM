#include "board.h"
#include "ui.h"
#include <lcd_sh1107.h>
#include <climits>
#include <cstdlib>
#include <fonts/font10.h>
#include <multimeter.h>
#include "multimeter_init.h"

enum MultimeterModes {FREQUENCY, RESISTANCE, DIODE_TEST, CONTINUITY, CAPACITANCE, INDUCTANCE};
#define MULTIMETER_MAX_MODE INDUCTANCE
#define MULTIMETER_MIN_MODE FREQUENCY

#define MAIN_FONT courierNew10ptFontInfo
#define VALUE_X (2*(MAIN_FONT.character_max_width+MAIN_FONT.character_spacing))
#define VDDA_X  (6*(MAIN_FONT.character_max_width+MAIN_FONT.character_spacing))
#define F_X     (MAIN_FONT.character_max_width+MAIN_FONT.character_spacing)

static MultimeterModes multimeter_mode = MULTIMETER_MIN_MODE;

multimeter_result_t multimeter_result =
{
  .frequency_hz = 0,
  .diode_voltage_uV = {0,0},
  .resistance_mOhm = {0, 0},
  .inductance_nH = 0,
  .temperature_Cx10 = 0,
  .vdda_mV = 0,
  .capacitance = {0, 0, 0},
  .voltage_current = {{0, 0}, {0, 0}}
};

static void DrawF()
{
  LcdDrawText(0, MAIN_FONT.char_height*7, "F  0.000000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
}

static void DrawEmptyLine()
{
  LcdDrawText(0, MAIN_FONT.char_height*8, "           ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
}

static void DrawMode()
{
  switch (multimeter_mode)
  {
    case FREQUENCY:
      DrawF();
      DrawEmptyLine();
      LcdDrawText(0, MAIN_FONT.char_height*9, "F| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, nullptr);
      break;
    case RESISTANCE:
    case CONTINUITY:
      LcdDrawText(0, MAIN_FONT.char_height*7, "1R---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
      LcdDrawText(0, MAIN_FONT.char_height*8, "2R---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
      if (multimeter_mode == RESISTANCE)
        LcdDrawText(0, MAIN_FONT.char_height*9, "R| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, nullptr);
      else
        LcdDrawText(0, MAIN_FONT.char_height*9, "I| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, nullptr);
      break;
    case DIODE_TEST:
      LcdDrawText(0, MAIN_FONT.char_height*7, "1D---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
      LcdDrawText(0, MAIN_FONT.char_height*8, "2D---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
      LcdDrawText(0, MAIN_FONT.char_height*9, "D| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, nullptr);
      break;
    case CAPACITANCE:
      LcdDrawText(0, MAIN_FONT.char_height*7, "C   0.000nF", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
      DrawEmptyLine();
      LcdDrawText(0, MAIN_FONT.char_height*9, "C| Z| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, nullptr);
      break;
    case INDUCTANCE:
      DrawF();
      LcdDrawText(0, MAIN_FONT.char_height*8, "L   0.000uH", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
      LcdDrawText(0, MAIN_FONT.char_height*9, "L| ?| ?| ?|", &MAIN_FONT, BLACK_COLOR, WHITE_COLOR, nullptr);
  }
}

void UI_Init()
{
  LcdInit();
  LcdScreenFill(BLACK_COLOR);
  int y = 0;

  LcdDrawText(0, y, "00.0C 0.00V", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);

  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1U- 0.00000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1I-  0.0000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "1P 0.000000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);

  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2U- 0.00000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2I-  0.0000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  y += MAIN_FONT.char_height;
  LcdDrawText(0, y, "2P 0.000000", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);

  DrawMode();

  LcdUpdate();
}

static void DrawVoltage(const int line, const int value_uV)
{
  if (value_uV == INT_MAX)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "OVERVOLT ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  else if (value_uV == INT_MIN)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "UNDERVOLT", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  else
    LcdPrintf("%3d.%05d", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_uV / 1000000, abs(value_uV/10) % 100000);
}

static void DrawCurrent(const int line, const int value_nA)
{
  if (value_nA == INT_MAX)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "OVERCURR ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  else if (value_nA == INT_MIN)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "UNDERCURR", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  LcdPrintf("%3d.%04d", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
            value_nA / 1000000, abs(value_nA/100) % 10000);
}

static void DrawPower(const int line, int channel)
{
  int uV = multimeter_result.voltage_current[channel].voltage_uV;
  int nA = multimeter_result.voltage_current[channel].current_nA;

  if (uV == INT_MAX || uV == INT_MIN || nA == INT_MAX || nA == INT_MIN)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "OVERLOAD ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  else
  {
    if (uV < 0)
      uV = -uV;
    if (nA < 0)
      nA = -nA;
    const int uWatt = static_cast<int>(static_cast<long long int>(uV) * static_cast<long long int>(nA) / 1000000000LL);
    LcdPrintf("%2d.%06d", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              uWatt / 1000000, abs(uWatt) % 1000000);
  }
}

static void DrawTemperature(unsigned int value)
{
  if (value > 999)
    value = 999;
  LcdPrintf("%2d.%d", 0, 0, &MAIN_FONT, 1, value / 10, value % 10);
}

static void DrawVdda(unsigned int value_mV)
{
  if (value_mV > 9999)
    value_mV = 9999;
  LcdPrintf("%d.%02d", VDDA_X, 0, &MAIN_FONT, 1, value_mV / 1000, (value_mV/10) % 100);
}

static void DrawFrequency(const int line, unsigned int value)
{
  if (value > 999999999)
    value = 999999999;
  LcdPrintf("%3d.%06d", F_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1, value / 1000000, value % 1000000);
}

static void DrawInductance(const int line, const unsigned int value_nH)
{
  if (value_nH == UINT_MAX)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "ERROR    ", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  else if (value_nH < 1000000)
    LcdPrintf("%3d.%03dn", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_nH / 1000, value_nH % 1000);
  else
    LcdPrintf("%3d.%03dm", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_nH / 1000000, (value_nH/1000) % 1000);
}

static void DrawCapacitance(const int line, const unsigned int value_pF)
{
  if (value_pF < 10000000)
    LcdPrintf("%4d.%03dn", F_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_pF / 1000, value_pF % 1000);
  else
    LcdPrintf("%4d.%03du", F_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_pF / 1000000, (value_pF/1000) % 1000);
}

static void DrawResistance(const int line, const unsigned int value_mOhm)
{
  if (value_mOhm == UINT_MAX)
    LcdDrawText(VALUE_X, line*MAIN_FONT.char_height, "---------", &MAIN_FONT, WHITE_COLOR, BLACK_COLOR, nullptr);
  else if (value_mOhm < 10000000)
    LcdPrintf("%4d.%03d ", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_mOhm / 1000, value_mOhm % 1000);
  else
    LcdPrintf("%4d.%03dK", VALUE_X, line*MAIN_FONT.char_height, &MAIN_FONT, 1,
              value_mOhm / 1000000, (value_mOhm/1000) % 1000);
}

/*
 * L = (NOM / F / F / C) * 100 / PI2
 * C in pF
 * L in uH
 * F in HZ
 */
#define NOM (2500000000 * 100000000)
#define PI2 987
#define DEFAULT_L_CAP 3400 //pF
static unsigned int calculate_inductance(unsigned long long int frequency)
{
  unsigned long long int l;
  unsigned long long int f2;
  unsigned long long int c = DEFAULT_L_CAP / 100;
  if (!frequency)
    return UINT_MAX;
  f2 = frequency * frequency;
  l = NOM / f2 / c;
  return l * 100 / PI2;
}

void multimeter_set_mode(const MultimeterModes mode)
{
  multimeter_mode = mode;
  switch (mode)
  {
    case FREQUENCY:
    case INDUCTANCE:
      multimeter.EnableChannels(DEFAULT_ENABLED_MEASUREMENTS);
      break;
    case RESISTANCE:
    case CONTINUITY:
    case DIODE_TEST:
      multimeter.EnableChannels(ALWAYS_ENABLED_MEASUREMENTS | (1<<CHANNEL_TYPE_RESISTANCE));
      break;
    case CAPACITANCE:
      multimeter.EnableChannels(ALWAYS_ENABLED_MEASUREMENTS | (1<<CHANNEL_TYPE_CAPACITANCE));
  }
}
void Process_Timer_Event(const unsigned char keyboard_status)
{
  MultimeterChannelType channel_type;
  int channel;
  bool multimeter_changes = false;
  bool power_changed[2] = {false, false};
  for (;;)
  {
    int result = multimeter.GetMeasurementResult(&channel_type, &channel);
    switch (channel_type)
    {
      CHANNEL_TYPE_FREQUENCY:
        if (multimeter_mode == FREQUENCY)
        {
          DrawFrequency(7, result);
          if (multimeter_mode == INDUCTANCE)
            DrawInductance(8, calculate_inductance(result));
          multimeter_changes = true;
        }
        break;
      CHANNEL_TYPE_VOLTAGE:
        DrawVoltage(channel == 0 ? 1 : 4, result);
        power_changed[channel] = true;
        multimeter_result.voltage_current[channel].voltage_uV = result;
        multimeter_changes = true;
        break;
      CHANNEL_TYPE_CURRENT:
        DrawCurrent(channel == 0 ? 2 : 5, result);
        power_changed[channel] = true;
        multimeter_result.voltage_current[channel].current_nA = result;
        multimeter_changes = true;
        break;
      CHANNEL_TYPE_VDDA:
        DrawVdda(result);
        multimeter_changes = true;
        break;
      CHANNEL_TYPE_TEMPERATURE:
        DrawTemperature(result);
        multimeter_changes = true;
        break;
      CHANNEL_TYPE_CAPACITANCE:
        if (multimeter_mode == CAPACITANCE)
        {
          DrawCapacitance(7, result);
          multimeter_changes = true;
        }
        break;
      CHANNEL_TYPE_RESISTANCE:
        if (multimeter_mode == RESISTANCE || multimeter_mode == CONTINUITY || multimeter_mode == DIODE_TEST)
        {
          DrawResistance(channel == 0 ? 7 : 8, result);
          multimeter_changes = true;
        }
        break;
      default:
        goto exit;
    }
    /*if (multimeter_changes & DIODE_VOLTAGE1_CHANGED && multimeter_mode == DIODE_TEST)
      DrawVoltage(7, (int)multimeter_result.diode_voltage_uV[0]);
    if (multimeter_changes & DIODE_VOLTAGE2_CHANGED && multimeter_mode == DIODE_TEST)
      DrawVoltage(8, (int)multimeter_result.diode_voltage_uV[1]);*/
  }

exit:
  if (power_changed[0])
    DrawPower(3, 0);
  if (power_changed[1])
    DrawPower(6, 1);

  switch (keyboard_status)
  {
    case KB_MODE:
      if (multimeter_mode == MULTIMETER_MAX_MODE)
        multimeter_set_mode(MULTIMETER_MIN_MODE);
      else
        multimeter_set_mode(static_cast<MultimeterModes>(multimeter_mode + 1));
      DrawMode();
      multimeter_changes = true;
      break;
    case KB_MODE_LONG:
      power_off();
      break;
    default:
      break;
  }

  if (multimeter_changes)
    LcdUpdate();
}
