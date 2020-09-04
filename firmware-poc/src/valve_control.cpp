#include <Arduino.h>
#include "valve_control.h"

#define valve_pin 14

void valve_init(){
    pinMode(valve_pin, OUTPUT);
}

void valve_open(){
    digitalWrite(valve_pin, HIGH);
}

void valve_close(){
    digitalWrite(valve_pin, LOW);
}