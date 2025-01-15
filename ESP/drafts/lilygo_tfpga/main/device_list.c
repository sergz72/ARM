#include "device_list.h"
#include "dev_ina3221.h"
#include "dev_ina226.h"
#include "dev_ads1115.h"
#include "dev_ad9833.h"
#include "dev_si5351.h"
#include "dev_mcp3421.h"
#include "dev_dds.h"

const Device devices[] = {
    {
        .name = "ina3221",
        .device_id = INA3221_DEVICE_ID,
        .initializer = ina3221_initializer,
        .data_collector = ina3221_data_collector
    },
    {
        .name = "ina226",
        .device_id = INA226_DEVICE_ID,
        .initializer = ina226_initializer,
        .data_collector = ina226_data_collector
    },
    {
        .name = "ads1115",
        .device_id = ADS1115_DEVICE_ID,
        .initializer = ads1115_initializer,
        .data_collector = ads1115_data_collector
    },
    {
        .name = "mcp3421",
        .device_id = MCP3421_DEVICE_ID,
        .initializer = mcp3421_initializer,
        .data_collector = mcp3421_data_collector
    },
    {
        .name = "ad9833",
        .device_id = AD9833_DEVICE_ID,
        .initializer = ad9833_initializer,
        .data_collector = NULL
    },
    {
        .name = "si5351",
        .device_id = SI5351_DEVICE_ID,
        .initializer = si5351_initializer,
        .data_collector = NULL
    }
};
