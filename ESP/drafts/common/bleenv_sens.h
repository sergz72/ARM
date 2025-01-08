#ifndef H_BLEENV_SENSOR_
#define H_BLEENV_SENSOR_

#include "nimble/ble.h"
#include "modlog/modlog.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Heart-rate configuration */
#define GATT_EVS_UUID                           0x181A
#define GATT_EVS_WIFI_INFO_UUID                 0x181C
#define GATT_EVS_WIFI_NAME_UUID                 0x2A8A
#define GATT_EVS_WIFI_PWD_UUID                  0x2A90
#define GATT_TEMP_UUID                          0x2A6E
#define GATT_HUMI_UUID                          0x2A6F
#define GATT_PRES_UUID                          0x2A6D
#define GATT_DEVICE_INFO_UUID                   0x180A
#define GATT_MANUFACTURER_NAME_UUID             0x2A29
#define GATT_MODEL_NUMBER_UUID                  0x2A24

//extern uint16_t evs_hrm_handle;

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

int gatt_svr_init(void);

#ifdef __cplusplus
}
#endif

#endif
