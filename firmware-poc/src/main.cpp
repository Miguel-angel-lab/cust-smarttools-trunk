#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "pressure_handling.h"
#include "valve_control.h"
#include "pump_control.h"
#include "buttons.h"
#include "espDelay.h"
#include "display.h"


float test = 0;
bool test_loop = false;


void setup()
{
    Serial.begin(115200);

    Serial.println("Display Init");
    display_init();
    Serial.println("Display Init Complete");

    Serial.println("Pressure Sensor Init");
    pressure_sensor_init();
    Serial.println("PressureSensor Init Complete");

    Serial.println("Valve Control Init");
    valve_init();
    Serial.println("Valve Control Init Complete");

    Serial.println("Pump Control Init");
    pump_init();
    Serial.println("Pump Control Init Complete");

    Serial.println("Calibration Init");
    pressure_sensor_calibrate();
    Serial.println("Calibration Complete");

    Serial.println("Pump Test");
    pump_test();
    Serial.println("Pump Test Complete");

    Serial.println("Buttons Init");
    buttons_init();
    Serial.println("Buttons Init Complete");




}



void loop()
{

test = pressure_sensor_read();
display_pressure(test);
if(!test_loop){
pressure_find_occlusion();
}
test_loop = true;



}