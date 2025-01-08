#include <assert.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "bleenv_sens.h"
#include "env.h"
#include "wifi.h"
#include "nvs.h"

static const char *manuf_name = "M5Stack C3U";
static const char *model_num = "ENV Sensor";

static int gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                           struct ble_gatt_access_ctxt *ctxt, void *arg);
#ifdef BLE_ENV
static int gatt_svr_chr_get_env(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg);
#endif
static int gatt_svr_chr_set_wifi_name(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctxt, void *arg);
static int gatt_svr_chr_set_wifi_pwd(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
#ifdef BLE_ENV
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_EVS_UUID),
        .characteristics = (struct ble_gatt_chr_def[])
        {
          {
                .uuid = BLE_UUID16_DECLARE(GATT_TEMP_UUID),
                .access_cb = gatt_svr_chr_get_env,
                .flags = BLE_GATT_CHR_F_READ,
              },
          {
                .uuid = BLE_UUID16_DECLARE(GATT_HUMI_UUID),
                .access_cb = gatt_svr_chr_get_env,
                .flags = BLE_GATT_CHR_F_READ,
              },
#ifdef SEND_PRESSURE
          {
                .uuid = BLE_UUID16_DECLARE(GATT_PRES_UUID),
                .access_cb = gatt_svr_chr_get_env,
                .flags = BLE_GATT_CHR_F_READ,
              },
#endif
          {
                0, /* No more characteristics in this service */
              },
        }
    },
#endif
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_EVS_WIFI_INFO_UUID),
        .characteristics = (struct ble_gatt_chr_def[])
            { {
                  .uuid = BLE_UUID16_DECLARE(GATT_EVS_WIFI_NAME_UUID),
                  .access_cb = gatt_svr_chr_set_wifi_name,
                  .flags = BLE_GATT_CHR_F_WRITE,
              }, {
                  .uuid = BLE_UUID16_DECLARE(GATT_EVS_WIFI_PWD_UUID),
                  .access_cb = gatt_svr_chr_set_wifi_pwd,
                  .flags = BLE_GATT_CHR_F_WRITE,
              }, {
                  0, /* No more characteristics in this service */
              },
            }
    },

    {
        /* Service: Device Information */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /* Characteristic: * Manufacturer name */
                .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
                .access_cb = gatt_svr_chr_access_device_info,
                .flags = BLE_GATT_CHR_F_READ,
            }, {
                /* Characteristic: Model number string */
                .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
                .access_cb = gatt_svr_chr_access_device_info,
                .flags = BLE_GATT_CHR_F_READ,
            }, {
                0, /* No more characteristics in this service */
            },
        }
    },

    {
        0, /* No more services */
    },
};

#ifdef BLE_ENV
static int gatt_svr_chr_get_env(uint16_t conn_handle, uint16_t attr_handle,
                                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
  uint16_t uuid;
  int rc;

  uuid = ble_uuid_u16(ctxt->chr->uuid);

  if (uuid == GATT_TEMP_UUID)
  {
    rc = os_mbuf_append(ctxt->om, &temp_val,
                        sizeof temp_val);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
#ifdef SEND_PRESSURE
  if (uuid == GATT_PRES_UUID)
  {
    rc = os_mbuf_append(ctxt->om, &pres_val,
                        sizeof pres_val);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
#endif
  if (uuid == GATT_HUMI_UUID)
  {
    rc = os_mbuf_append(ctxt->om, &humi_val,
                        sizeof humi_val);
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }
  return 0;
}
#endif

static int gatt_svr_chr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
                              void *dst, uint16_t *len)
{
  uint16_t om_len;
  int rc;

  om_len = OS_MBUF_PKTLEN(om);
  if (om_len < min_len || om_len > max_len) {
    return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
  }

  rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
  if (rc != 0) {
    return BLE_ATT_ERR_UNLIKELY;
  }

  return 0;
}

static int gatt_svr_chr_set_wifi_name(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
  uint16_t len;
  int rc;

  rc = gatt_svr_chr_write(ctxt->om,
                          1,
                          WIFI_NAME_LENGTH,
                          wifi_config.sta.ssid, &len);
  if (rc == 0)
  {
    wifi_config.sta.ssid[len] = 0;
    MODLOG_DFLT(DEBUG, "wifi_name %s", wifi_config.sta.ssid);
    nvs_set_wifi_name();
  } else {
    MODLOG_DFLT(ERROR, "gatt_svr_chr_set_wifi_name error %d", rc);
  }

  return rc;
}

static int gatt_svr_chr_set_wifi_pwd(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
  uint16_t len;
  int rc;

  rc = gatt_svr_chr_write(ctxt->om,
                          1,
                          WIFI_PASSWORD_LENGTH,
                          wifi_config.sta.password, &len);
  if (rc == 0)
  {
    wifi_config.sta.password[len] = 0;
    MODLOG_DFLT(DEBUG, "wifi_pwd %s", wifi_config.sta.password);
    nvs_set_wifi_pwd();
  } else {
    MODLOG_DFLT(ERROR, "gatt_svr_chr_set_wifi_pwd error %d", rc);
  }

  return rc;
}

static int gatt_svr_chr_access_device_info(uint16_t conn_handle, uint16_t attr_handle,
                                           struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    uint16_t uuid;
    int rc;

    uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == GATT_MODEL_NUMBER_UUID) {
        rc = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    if (uuid == GATT_MANUFACTURER_NAME_UUID) {
        rc = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

int gatt_svr_init(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    return 0;
}
