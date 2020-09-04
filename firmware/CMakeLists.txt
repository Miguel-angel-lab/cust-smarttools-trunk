cmake_minimum_required(VERSION 3.16)

# Hack to get custom sdkconfig files to work
# Setting SDKCONFIG does not fix it for kconfig_new/kconfiglib.py which makes the file sdkconfig anyway

if ($ENV{CLION_IDE})
    add_definitions(-DSDKCONFIG=${SDKCONFIG})
    set(ENV{SDKCONFIG} sdkconfig.${PIO_ENV_NAME})
else()
    get_filename_component(PIO_ENV_NAME ${CMAKE_BINARY_DIR} NAME)
    set(SDKCONFIG_ALT sdkconfig.${PIO_ENV_NAME})
    if(NOT EXISTS ${SDKCONFIG_ALT})
        message(WARNING "${SDKCONFIG_ALT} not found. Will copy from sdkconfig.prod.")
        file(COPY sdkconfig.prod DESTINATION ${SDKCONFIG_ALT})
    endif()
    file(REMOVE ${CMAKE_CURRENT_LIST_DIR}/sdkconfig)
    file(CREATE_LINK ${SDKCONFIG_ALT} ${CMAKE_CURRENT_LIST_DIR}/sdkconfig SYMBOLIC)
endif()

# CLion-specific hacks
# TODO Check that file exists--`AND EXISTS "CMakeLists.CLion.cmake` doesn't work
if ($ENV{CLION_IDE})
    include(CMakeLists.CLion.cmake)
endif()


set(CMAKE_CXX_STANDARD 17)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Arduino variables needed for Arduino libraries to function correctly
# For whatever reason these are not set by PIO when using both ESP-IDF and Arduino frameworks at the same time.
add_definitions(-DESP32)
add_definitions(-DESP_PLATFORM)
add_definitions(-DF_CPU=240000000L)
add_definitions(-DARDUINO=10805)
add_definitions(-DARDUINO_ARCH_ESP32)
add_definitions(-DARDUINO_VARIANT="esp32")
add_definitions(-DPIO_FRAMEWORK_ESP_IDF_ENABLE_EXCEPTIONS)

# ESP-IDF uses `main` as the default source directory, so we have to register `src` as a component for it to be found.
# Setting PIO to look in `main` causes the Arduino framework to not load correctly.
list(APPEND EXTRA_COMPONENT_DIRS src)
list(APPEND EXTRA_COMPONENT_DIRS lib/Adafruit_MPRLS)
list(APPEND EXTRA_COMPONENT_DIRS lib/lv_port_esp32/components/lvgl)
list(APPEND EXTRA_COMPONENT_DIRS lib/lv_port_esp32/components/lvgl_esp32_drivers/lvgl_tft)
list(APPEND EXTRA_COMPONENT_DIRS lib/lv_port_esp32/components/lvgl_esp32_drivers)

include_directories(
    lib/rxcpp/Rx/v2/src
    lib/Adafruit_MPRLS
    include
)

project(firmware C CX CXX)
