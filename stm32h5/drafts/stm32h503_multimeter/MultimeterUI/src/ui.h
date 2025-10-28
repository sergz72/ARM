#ifndef _MULTIMETER_UI_H
#define _MULTIMETER_UI_H

typedef struct
{
  unsigned int pF;
  unsigned int diff;
  int channel;
} capacitance_result;

typedef struct
{
  int voltage_uV;
  int current_nA;
} voltage_current_result;

typedef struct
{
  unsigned int frequency_hz;
  unsigned int diode_voltage_uV[2];
  unsigned int resistance_mOhm[2];
  unsigned int temperature_Cx10;
  unsigned int vdda_uV;
  capacitance_result capacitance;
  voltage_current_result voltage_current[2];
} multimeter_result_t;

void UI_Init();
void Process_Timer_Event(unsigned char keyboard_status);

#endif
