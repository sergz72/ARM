#ifndef PICO_METER_MAX7219_DEV_MCP9600_H
#define PICO_METER_MAX7219_DEV_MCP9600_H

#define MCP9600_DEVICE_ID 0xCE

void* mcp9600_initializer(void);
void mcp9600_ui_init_handler(void* config);
void *mcp9600_data_collector(int step, void* config, void *data);
void mcp9600_ui_handler(void* data, void* config);
int mcp9600_ui_keyboard_handler(void *config, unsigned int event);

#endif //PICO_METER_MAX7219_DEV_MCP9600_H
