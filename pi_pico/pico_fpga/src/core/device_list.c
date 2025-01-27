#include "device_list.h"
#include "dev_ina3221.h"
#include "dev_ina226.h"
#include "dev_ads1115.h"
#include "dev_ad9833.h"
#include "dev_si5351.h"
#include "dev_mcp3421.h"
#include "dev_mcp9600.h"
#include "dev_dds.h"

#define PUBLIC_ID_DDS 1
#define PUBLIC_ID_VOLTMETER 2
#define PUBLIC_ID_POWER_METER 3
#define PUBLIC_ID_THERMOMETER 4

const Device devices[] = {
    {
        .name = "ina3221",
        .device_id = INA3221_DEVICE_ID,
        .public_id = PUBLIC_ID_POWER_METER,
        .initializer = ina3221_initializer,
        .timer_event = ina3221_timer_event,
        .message_processor = NULL,
        .save_config = ina3221_save_config
    },
    {
        .name = "ina226",
        .device_id = INA226_DEVICE_ID,
        .public_id = PUBLIC_ID_POWER_METER,
        .initializer = ina226_initializer,
        .timer_event = ina226_timer_event,
        .message_processor = NULL,
        .save_config = ina226_save_config
    },
    {
        .name = "ads1115",
        .device_id = ADS1115_DEVICE_ID,
        .public_id = PUBLIC_ID_VOLTMETER,
        .initializer = ads1115_initializer,
        .timer_event = ads1115_timer_event,
        .message_processor = NULL,
        .save_config = ads1115_save_config
    },
    {
        .name = "mcp3421",
        .device_id = MCP3421_DEVICE_ID,
        .public_id = PUBLIC_ID_VOLTMETER,
        .initializer = mcp3421_initializer,
        .timer_event = mcp3421_timer_event,
        .message_processor = NULL,
        .save_config = mcp3421_save_config
    },
    {
        .name = "ad9833",
        .device_id = AD9833_DEVICE_ID,
        .public_id = PUBLIC_ID_DDS,
        .initializer = ad9833_initializer,
        .timer_event = NULL,
        .message_processor = dds_message_processor,
        .save_config = ad9833_save_config
    },
    {
        .name = "si5351",
        .device_id = SI5351_DEVICE_ID,
        .public_id = PUBLIC_ID_DDS,
        .initializer = si5351_initializer,
        .timer_event = NULL,
        .message_processor = dds_message_processor,
        .save_config = si5351_save_config
    },
    {
        .name = "mcp9600",
        .device_id = MCP9600_DEVICE_ID,
        .public_id = MCP9600_DEVICE_ID,
        .initializer = mcp9600_initializer,
        .timer_event = mcp9600_timer_event,
        .message_processor = NULL,
        .save_config = mcp9600_save_config
    }
};
