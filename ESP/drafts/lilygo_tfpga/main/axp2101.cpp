#include <driver/i2c_master.h>
#include <esp_log.h>
#include <esp_err.h>
#define XPOWERS_CHIP_AXP2101
#include <XPowersLib.h>
#include "pmu.h"

static const char *TAG = "AXP2101";

static XPowersPMU power;

esp_err_t pmu_init()
{
    if (power.begin(bus_handle, AXP2101_SLAVE_ADDRESS)) {
        ESP_LOGI(TAG, "Init PMU SUCCESS!");
    } else {
        ESP_LOGE(TAG, "Init PMU FAILED!");
        return ESP_FAIL;
    }

    //Turn off not use power channel
    power.disableDC2();
    power.disableDC3();
    power.disableCPUSLDO();

    power.setDC4Voltage(1200);   // Here is the FPGA core voltage. Careful review of the manual is required before modification.
    power.setALDO1Voltage(3300); // BANK0 area voltage
    power.setALDO2Voltage(3300); // BANK1 area voltage
    power.setALDO3Voltage(3300); // BANK2 area voltage
    power.setALDO4Voltage(1800); // BANK3 area voltage
    power.setBLDO1Voltage(3300);
    power.setBLDO2Voltage(3300);

    power.enableDC4();
    power.enableALDO1();
    power.enableALDO2();
    power.enableALDO3();
    power.enableALDO4();
    power.enableBLDO1();
    power.enableBLDO2();

    ESP_LOGI(TAG, "DCDC=======================================================================\n");
    ESP_LOGI(TAG, "DC1  : %s   Voltage:%u mV \n",  power.isEnableDC1()  ? "+" : "-", power.getDC1Voltage());
    ESP_LOGI(TAG, "DC2  : %s   Voltage:%u mV \n",  power.isEnableDC2()  ? "+" : "-", power.getDC2Voltage());
    ESP_LOGI(TAG, "DC3  : %s   Voltage:%u mV \n",  power.isEnableDC3()  ? "+" : "-", power.getDC3Voltage());
    ESP_LOGI(TAG, "DC4  : %s   Voltage:%u mV \n",  power.isEnableDC4()  ? "+" : "-", power.getDC4Voltage());
    ESP_LOGI(TAG, "ALDO=======================================================================\n");
    ESP_LOGI(TAG, "ALDO1: %s   Voltage:%u mV\n",  power.isEnableALDO1()  ? "+" : "-", power.getALDO1Voltage());
    ESP_LOGI(TAG, "ALDO2: %s   Voltage:%u mV\n",  power.isEnableALDO2()  ? "+" : "-", power.getALDO2Voltage());
    ESP_LOGI(TAG, "ALDO3: %s   Voltage:%u mV\n",  power.isEnableALDO3()  ? "+" : "-", power.getALDO3Voltage());
    ESP_LOGI(TAG, "ALDO4: %s   Voltage:%u mV\n",  power.isEnableALDO4()  ? "+" : "-", power.getALDO4Voltage());
    ESP_LOGI(TAG, "BLDO=======================================================================\n");
    ESP_LOGI(TAG, "BLDO1: %s   Voltage:%u mV\n",  power.isEnableBLDO1()  ? "+" : "-", power.getBLDO1Voltage());
    ESP_LOGI(TAG, "BLDO2: %s   Voltage:%u mV\n",  power.isEnableBLDO2()  ? "+" : "-", power.getBLDO2Voltage());
    ESP_LOGI(TAG, "CPUSLDO====================================================================\n");
    ESP_LOGI(TAG, "CPUSLDO: %s Voltage:%u mV\n",  power.isEnableCPUSLDO() ? "+" : "-", power.getCPUSLDOVoltage());
    ESP_LOGI(TAG, "DLDO=======================================================================\n");
    ESP_LOGI(TAG, "DLDO1: %s   Voltage:%u mV\n",  power.isEnableDLDO1()  ? "+" : "-", power.getDLDO1Voltage());
    ESP_LOGI(TAG, "DLDO2: %s   Voltage:%u mV\n",  power.isEnableDLDO2()  ? "+" : "-", power.getDLDO2Voltage());
    ESP_LOGI(TAG, "===========================================================================\n");

    power.clearIrqStatus();

    power.enableVbusVoltageMeasure();
    power.enableBattVoltageMeasure();
    power.enableSystemVoltageMeasure();
    power.enableTemperatureMeasure();

    // It is necessary to disable the detection function of the TS pin on the board
    // without the battery temperature detection function, otherwise it will cause abnormal charging
    power.disableTSPinMeasure();

    // Disable all interrupts
    power.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    // Clear all interrupt flags
    power.clearIrqStatus();
    // Enable the required interrupt function
    /*power.enableIRQ(
        XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
        // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
    );*/

    /*
      The default setting is CHGLED is automatically controlled by the power.
    - XPOWERS_CHG_LED_OFF,
    - XPOWERS_CHG_LED_BLINK_1HZ,
    - XPOWERS_CHG_LED_BLINK_4HZ,
    - XPOWERS_CHG_LED_ON,
    - XPOWERS_CHG_LED_CTRL_CHG,
    * */
    power.setChargingLedMode(XPOWERS_CHG_LED_CTRL_CHG);

    // Set the precharge charging current
    power.setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_50MA);
    // Set constant current charge current limit
    power.setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_200MA);
    // Set stop charging termination current
    power.setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);

    // Set charge cut-off voltage
    power.setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V1);

    // Set the watchdog trigger event type
    // power.setWatchdogConfig(XPOWERS_AXP2101_WDT_IRQ_TO_PIN);
    // Set watchdog timeout
    //power.setWatchdogTimeout(XPOWERS_AXP2101_WDT_TIMEOUT_4S);
    // Enable watchdog to trigger interrupt event
    //power.enableWatchdog();

    return ESP_OK;
}
