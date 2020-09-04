idf_component_register(SRCS main.cc
        INCLUDE_DIRS .
        REQUIRES lvgl_esp32_drivers lvgl lvgl_tft)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
