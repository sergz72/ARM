#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "bleenv_sens.h"
#include "led.h"
#include "common.h"

static uint8_t ble_addr_type;

static int bleenv_gap_event(struct ble_gap_event *event, void *arg);

static const char *device_name = DEVICE_NAME;

static uint16_t conn_handle;

static const char *TAG = "ble";

void print_addr(const void *addr)
{
  const uint8_t *u8p;

  u8p = addr;
  MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x",
              u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}

static void ble_advertise(void)
{
  struct ble_gap_adv_params adv_params;
  struct ble_hs_adv_fields fields;
  int rc;

  /*
   *  Set the advertisement data included in our advertisements:
   *     o Flags (indicates advertisement type and other general info)
   *     o Advertising tx power
   *     o Device name
   */
  memset(&fields, 0, sizeof(fields));

  /*
   * Advertise two flags:
   *      o Discoverability in forthcoming advertisement (general)
   *      o BLE-only (BR/EDR unsupported)
   */
  fields.flags = BLE_HS_ADV_F_DISC_GEN |
                 BLE_HS_ADV_F_BREDR_UNSUP;

  /*
   * Indicate that the TX power level field should be included; have the
   * stack fill this value automatically.  This is done by assigning the
   * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
   */
  fields.tx_pwr_lvl_is_present = 1;
  fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

  fields.name = (uint8_t *)device_name;
  fields.name_len = strlen(device_name);
  fields.name_is_complete = 1;

  rc = ble_gap_adv_set_fields(&fields);
  if (rc != 0)
  {
    MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
    return;
  }

  /* Begin advertising */
  memset(&adv_params, 0, sizeof(adv_params));
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
  rc = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER,
                         &adv_params, bleenv_gap_event, NULL);
  if (rc != 0)
  {
    MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
    return;
  }
}

static int bleenv_gap_event(struct ble_gap_event *event, void *arg)
{
  switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
      /* A new connection was established or a connection attempt failed */
      MODLOG_DFLT(INFO, "connection %s; status=%d\n",
                  event->connect.status == 0 ? "established" : "failed",
                  event->connect.status);

      if (event->connect.status != 0) {
        /* Connection failed; resume advertising */
        ble_advertise();
      }
      conn_handle = event->connect.conn_handle;
      break;

    case BLE_GAP_EVENT_DISCONNECT:
      MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);

      /* Connection terminated; resume advertising */
      ble_advertise();
      break;

    case BLE_GAP_EVENT_ADV_COMPLETE:
      MODLOG_DFLT(INFO, "adv complete\n");
      ble_advertise();
      break;

    case BLE_GAP_EVENT_SUBSCRIBE:
      MODLOG_DFLT(INFO, "subscribe event; cur_notify=%d\n value handle; "
                        "val_handle=%d\n",
                  event->subscribe.cur_notify, 0);//hrs_hrm_handle);
      /*if (event->subscribe.attr_handle == hrs_hrm_handle) {
        notify_state = event->subscribe.cur_notify;
        blehr_tx_hrate_reset();
      } else if (event->subscribe.attr_handle != hrs_hrm_handle) {
        notify_state = event->subscribe.cur_notify;
        blehr_tx_hrate_stop();
      }*/
      ESP_LOGI("BLE_GAP_SUBSCRIBE_EVENT", "conn_handle from subscribe=%d", conn_handle);
      break;

    case BLE_GAP_EVENT_MTU:
      MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d mtu=%d\n",
                  event->mtu.conn_handle,
                  event->mtu.value);
      break;

  }

  return 0;
}

static void ble_on_sync(void)
{
  int rc;

  rc = ble_hs_id_infer_auto(0, &ble_addr_type);
  if (rc != 0)
  {
    ESP_LOGE(TAG, "ble_hs_id_infer_auto failed");
    set_led_red();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_led_yellow();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_led_red();
    while (1){}
  }

  uint8_t addr_val[6] = {0};
  ble_hs_id_copy_addr(ble_addr_type, addr_val, NULL);

  MODLOG_DFLT(INFO, "Device Address: ");
  print_addr(addr_val);
  MODLOG_DFLT(INFO, "\n");

  /* Begin advertising */
  ble_advertise();
}

static void ble_on_reset(int reason)
{
  MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

static void ble_host_task(void *param)
{
  ESP_LOGI(TAG, "BLE Host Task Started");
  /* This function will return only when nimble_port_stop() is executed */
  nimble_port_run();

  nimble_port_freertos_deinit();
}

void ble_init(void)
{
  int rc;

  nimble_port_init();
  /* Initialize the NimBLE host configuration */
  ble_hs_cfg.sync_cb = ble_on_sync;
  ble_hs_cfg.reset_cb = ble_on_reset;

  ESP_LOGI(TAG, "gatt_svr_init...");
  rc = gatt_svr_init();
  if (rc != 0)
  {
    ESP_LOGE(TAG, "gatt_svr_init failed");
    set_led_red();
    while (1){}
  }
  ESP_LOGI(TAG, "ble_svc_gap_device_name_set...");
  /* Set the default device name */
  rc = ble_svc_gap_device_name_set(device_name);
  if (rc != 0)
  {
    ESP_LOGE(TAG, "ble_svc_gap_device_name_set failed");
    set_led_red();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_led_green();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    set_led_red();
    while (1){}
  }

  nimble_port_freertos_init(ble_host_task);
}