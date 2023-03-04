#include <stddef.h>
#include "device_list.h"
#include "dev_ina226.h"
#include "dev_si5351.h"
#include "dev_mcp3421.h"
#include "dev_freq_pwm.h"
#include "dev_cap_meter.h"
#include "dev_voltmeter.h"
#include "dev_l_meter.h"

const Device devices[] = {
    {
        .name = "cap_meter",
        .short_name = "CA",
        .initializer = cap_meter_initializer,
        .data_collector = NULL,
        .ui_init_handler = cap_meter_ui_init_handler,
        .ui_data_handler = cap_meter_ui_handler,
        .ui_keyboard_handler = cap_meter_ui_keyboard_handler,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
        .calibrate = NULL,
        .calibrate_parameter_count = 0,
        .calibrate_help = NULL
    },
    {
        .name = "l_meter",
        .short_name = "L ",
        .initializer = l_meter_initializer,
        .data_collector = NULL,
        .ui_init_handler = l_meter_ui_init_handler,
        .ui_data_handler = l_meter_ui_handler,
        .ui_keyboard_handler = l_meter_ui_keyboard_handler,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
        .calibrate = NULL,
        .calibrate_parameter_count = 0,
        .calibrate_help = NULL
    },
    {
        .name = "freq_pwm",
        .short_name = "FP",
        .initializer = freq_pwm_initializer,
        .data_collector = NULL,
        .ui_init_handler = freq_pwm_ui_init_handler,
        .ui_data_handler = freq_pwm_ui_handler,
        .ui_keyboard_handler = freq_pwm_ui_keyboard_handler,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
        .calibrate = freq_pwm_calibrate,
        .calibrate_parameter_count = 2,
        .calibrate_help = "f1 f2"
    },
    {
        .name = "voltmeter",
        .short_name = "UO",
        .initializer = voltmeter_initializer,
        .data_collector = voltmeter_data_collector,
        .ui_init_handler = voltmeter_ui_init_handler,
        .ui_data_handler = voltmeter_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
        .calibrate = voltmeter_calibrate,
        .calibrate_parameter_count = 1,
        .calibrate_help = "value_mv"
    },
    {
        .name = "ina226",
        .short_name = "In",
        .initializer = ina226_initializer,
        .data_collector = ina226_data_collector,
        .ui_init_handler = ina226_ui_init_handler,
        .ui_data_handler = ina226_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = NULL,
        .set_config = NULL,
        .set_config_parameter_count = 0,
        .set_config_help = NULL,
        .calibrate = NULL,
        .calibrate_parameter_count = 0,
        .calibrate_help = NULL
    },
    {
        .name = "mcp3421",
        .short_name = "MC",
        .initializer = mcp3421_initializer,
        .data_collector = mcp3421_data_collector,
        .ui_init_handler = NULL,
        .ui_data_handler = mcp3421_ui_handler,
        .ui_keyboard_handler = NULL,
        .print_config = msp3421_print_config,
        .set_config = msp3421_set_config,
        .set_config_parameter_count = 2,
        .set_config_help = "coef offset",
        .calibrate = mcp3421_calibrate,
        .calibrate_parameter_count = 1,
        .calibrate_help = "value_mv"
    },
    {
        .name = "si5351",
        .short_name = "SI",
        .initializer = si5351_initializer,
        .data_collector = NULL,
        .ui_init_handler = si5351_ui_init_handler,
        .ui_data_handler = NULL,
        .ui_keyboard_handler = si5351_ui_keyboard_handler,
        .print_config = si5351_print_config,
        .set_config = si5351_set_config,
        .set_config_parameter_count = 1,
        .set_config_help = "corr",
        .calibrate = NULL,
        .calibrate_parameter_count = 0,
        .calibrate_help = NULL
    }
};
