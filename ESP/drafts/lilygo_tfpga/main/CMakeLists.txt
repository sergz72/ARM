idf_component_register(
  SRCS "main.c"
        "axp2101.cpp"
        "pmu_i2c.c"
        "devices.c"
        "device_list.c"
        "../../../ARM_CLION/common_lib/i2c_soft.c"
        "../../../ARM_CLION/common_lib/sensor/ads1115.c"
        "../../../ARM_CLION/common_lib/sensor/mcp3421.c"
        "../../../ARM_CLION/common_lib/sensor/ina226.c"
        "../../../ESP_CLION_COMMON/XPowersLib/src/XPowersLibInterface.cpp"
  INCLUDE_DIRS
        "."
        "../../../ESP_CLION_COMMON/XPowersLib/src"
        "../../../ARM_CLION/common_lib"
        "../../../ARM_CLION/common_lib/sensor"
)

add_definitions(-DCONFIG_XPOWERS_ESP_IDF_NEW_API)