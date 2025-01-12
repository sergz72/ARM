idf_component_register(
  SRCS "main.c" "axp2101.cpp" "pmu_i2c.c"
        "../../../ESP_CLION_COMMON/XPowersLib/src/XPowersLibInterface.cpp"
  INCLUDE_DIRS "." "../../../ESP_CLION_COMMON/XPowersLib/src"
)

add_definitions(-DCONFIG_XPOWERS_ESP_IDF_NEW_API)