#include <Arduino.h>
#include "pump_control.h"
#include "valve_control.h"

#define pump_pin 26

void pump_init(){
    pinMode(pump_pin, OUTPUT);
}

void pump_on(){
    digitalWrite(pump_pin, HIGH);
    delay(10);
    digitalWrite(pump_pin,LOW);
    delay(2);
}

void pump_off(){
    digitalWrite(pump_pin, LOW);
}

void pump_test(){
    pump_on();
    delay(1000);
    pump_off();
    valve_open();
    delay(1000);
    valve_close();
}