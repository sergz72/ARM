#ifndef _MCP9600_H
#define _MCP9600_H

typedef struct {
  unsigned char thermocouple_type;
  unsigned char filter_coefficient;
} MCP9600SensorConfig;

typedef struct {
  unsigned char low_cold_juction_resolution;
  unsigned char adc_resolution;
  unsigned char burst_mode_samples;
  unsigned char mode;
} MCP9600DeviceConfig;

#define MCP9600_THERMOCOUPLE_TYPE_K 0
#define MCP9600_THERMOCOUPLE_TYPE_J 1
#define MCP9600_THERMOCOUPLE_TYPE_T 2
#define MCP9600_THERMOCOUPLE_TYPE_N 3
#define MCP9600_THERMOCOUPLE_TYPE_S 4
#define MCP9600_THERMOCOUPLE_TYPE_E 5
#define MCP9600_THERMOCOUPLE_TYPE_B 6
#define MCP9600_THERMOCOUPLE_TYPE_R 7

#define MCP9600_ADC_RESOLUTION_18 0
#define MCP9600_ADC_RESOLUTION_16 1
#define MCP9600_ADC_RESOLUTION_14 2
#define MCP9600_ADC_RESOLUTION_12 3

#define MCP9600_BURST_MODE_1   0
#define MCP9600_BURST_MODE_2   1
#define MCP9600_BURST_MODE_4   2
#define MCP9600_BURST_MODE_8   3
#define MCP9600_BURST_MODE_16  4
#define MCP9600_BURST_MODE_32  5
#define MCP9600_BURST_MODE_64  6
#define MCP9600_BURST_MODE_128 7

#define MCP9600_MODE_NORMAL   0
#define MCP9600_MODE_SHUTDOWN 1
#define MCP9600_MODE_BURST    2

#define MCP9600_STATUS_SHORT_CIRCUIT 0x20
#define MCP9600_STATUS_RANGE         0x10

int mcp9600Read8(int channel, unsigned char address, unsigned char reg, unsigned char *data);
int mcp9600Read16(int channel, unsigned char address, unsigned char reg, unsigned short *data);
int mcp9600Write(int channel, unsigned char address, unsigned char reg, unsigned char data);

int mcp9600SetSensorConfig(int channel, unsigned char address, const MCP9600SensorConfig *cfg);
int mcp9600SetDeviceConfig(int channel, unsigned char address, const MCP9600DeviceConfig *cfg);
int mcp9600GetHotJunctionTemperature(int channel, unsigned char address, int *temperature);
int mcp9600GetDeltaTemperature(int channel, unsigned char address, int *temperature);
int mcp9600GetColdJunctionTemperature(int channel, unsigned char address, int *temperature);
int mcp9600ReadDeviceID(int channel, unsigned char address, unsigned short *id);
int mcp9600GetStatus(int channel, unsigned char address, unsigned char *status);

#endif
