#ifndef STM32H503_MULTIMETER_MULTIMETER_H
#define STM32H503_MULTIMETER_MULTIMETER_H

#define CAPACITANCE_CHANNEL_1K   0
#define CAPACITANCE_CHANNEL_100K 1

#define VOLTAGE1_CHANGED 1
#define CURRENT1_CHANGED 2
#define VOLTAGE2_CHANGED 4
#define CURRENT2_CHANGED 8
#define TEMPERATURE_CHANGED 16
#define VDDA_CHANGED 32
#define FREQUENCY_CHANGED 64
#define RESISTANCE1_CHANGED 128
#define RESISTANCE2_CHANGED 256
#define INDUCTANCE_CHANGED 512
#define CAPACITANCE_CHANGED 1024
#define DIODE_VOLTAGE1_CHANGED 2048
#define DIODE_VOLTAGE2_CHANGED 4096

#define VOLTAGE1_MEASUREMENT 1
#define VOLTAGE2_MEASUREMENT 2
#define CURRENT1_MEASUREMENT 4
#define CURRENT2_MEASUREMENT 8
#define FREQUENCY_MEASUREMENT 16
#define CAPACITANCE_MEASUREMENT_1K 32
#define CAPACITANCE_MEASUREMENT_100K 64
#define RESISTANCE1_MEASUREMENT_HIGH 128
#define RESISTANCE2_MEASUREMENT_HIGH 256
#define RESISTANCE1_MEASUREMENT_MEDIUM 512
#define RESISTANCE2_MEASUREMENT_MEDIUM 1024
#define RESISTANCE1_MEASUREMENT_LOW 2048
#define RESISTANCE2_MEASUREMENT_LOW 4096
#define TEMPERATURE_MEASUREMENT 8192
#define VDDA_MEASUREMENT 16384
#define DIODE_VOLTAGE_MEASUREMENT1 32768
#define DIODE_VOLTAGE_MEASUREMENT2 65536

enum multimeter_modes {FREQUENCY, RESISTANCE, DIODE_TEST, CONTINUITY, CAPACITANCE, INDUCTANCE};
#define MULTIMETER_MAX_MODE INDUCTANCE
#define MULTIMETER_MIN_MODE FREQUENCY

enum resistance_measurements_modes {HIGH, MEDIUM, LOW};

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
  unsigned int inductance_nH;
  unsigned int temperature_Cx100;
  unsigned int vdda_mV;
  capacitance_result capacitance;
  voltage_current_result voltage_current[2];
} multimeter_result_t;

extern multimeter_result_t multimeter_result;
extern enum multimeter_modes multimeter_mode;

int get_capacitance(capacitance_result *result);
int multimeter_init(void);

unsigned int multimeter_timer_event(void);
void multimeter_set_mode(enum multimeter_modes mode);

unsigned int check_measurements_statuses(void);
void start_voltage_measurements(void);
void start_frequency_measurement(void);
void start_resistance_measurement(int channel, enum resistance_measurements_modes mode);
void start_capacitance_measurement(int channel);
void start_current_measurement(int channel);
unsigned int start_extra_measurements(int channel, int extra_measurement_no);
void start_diode_voltage_measurement(int channel);
unsigned int finish_voltage_measurement(int channel);
unsigned int finish_frequency_measurement(void);
unsigned int finish_resistance_measurement(int channel, int mode);
unsigned int finish_current_measurement(int channel);
void finish_capacitance_measurement(void);
unsigned int finish_capacitance_measurement_1k(void);
unsigned int finish_temperature_measurement(void);
unsigned int finish_vdda_measurement(void);
unsigned int finish_diode_voltage_measurement(int channel);

#endif