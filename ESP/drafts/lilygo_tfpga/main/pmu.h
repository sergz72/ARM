#ifndef _PMU_H
#define _PMU_H

extern i2c_master_bus_handle_t bus_handle;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t pmu_init();

#ifdef __cplusplus
}
#endif

#endif
