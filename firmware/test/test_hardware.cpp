#include <unity.h>
#include "ValveControl.cpp"
#include "PumpControl.cpp"
#include "PressureSensor.cpp"

void test_valve() {
    ValveControl valve{ 4 };
    printf("Opening valve for 5 seconds... \n");
    valve.open();
    delay(5000);
    valve.close();
    printf("Valve closed \n");
}

void test_pump() {
    PumpControl pump{ 2 };
    printf("Turning pump on for 5 seconds... \n");
    pump.on();
    delay(5000);
    pump.off();
    printf("Pump off \n");
}

void test_pressure_sensor() {
    try {
        PressureSensor pressure{ 21, 22 };
        auto val = pressure.read_hpa();
        TEST_ASSERT_FLOAT_IS_DETERMINATE(val);
     } catch (std::exception &e) {
        TEST_FAIL_MESSAGE(e.what());
    }
}

extern "C" void app_main() {
    UNITY_BEGIN();
    RUN_TEST(test_pressure_sensor);
    RUN_TEST(test_valve);
    RUN_TEST(test_pump);
    UNITY_END();
}
