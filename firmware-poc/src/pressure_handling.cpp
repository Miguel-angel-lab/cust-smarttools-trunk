#include "pressure_handling.h"
#include <Wire.h>
#include "Adafruit_MPRLS.h"
#include "valve_control.h"
#include "pump_control.h"
#include "buttons.h"
#include "display.h"

// You dont *need* a reset and EOC pin for most uses, so we set to -1 and don't connect
#define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
#define EOC_PIN    -1  // set to any GPIO pin to read end-of-conversion by pin
Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN);

float pressure_hPa = 0;
float pressure_mmHg = 0;
float calibration_offset = 0;


//Occlusion Variables
int occlusion_array_size = 200;
float occlusion_pressure = 0;
bool find_occlusion_pressure_flag = false;
float found_occlusion_pressure = 0;
int inflate_pressure_value = 70;
bool occlusion_start_flag = false;
bool occlusion_stop_flag = false;
int occlusion_pressure_array[200];
int sum_occlusion_array = 0;
int occlusion_array_difference_setpoint = 5;
int occlusion_array_difference = 0;
int max_occlusion_array_value = 0;
bool arterial_compression_flag = false;

void pressure_sensor_init(){
  Serial.println("Connection to Pressure Sensor");
    if (! mpr.begin()) {
    Serial.println("Failed to communicate with MPRLS sensor, check wiring?");
    while (1) {
      delay(10);
    }
    }
    Serial.println("Found MPRLS sensor");
    pressure_sensor_calibrate();
}

void pressure_sensor_calibrate(){
  valve_open();
  delay(3000);
  calibration_offset = mpr.readPressure()*0.75006156130264;
  valve_close();

}

float pressure_sensor_read(){
  pressure_hPa = mpr.readPressure();
  pressure_mmHg = (pressure_hPa * 0.75006156130264)-calibration_offset;
  display_pressure(pressure_mmHg);
  //Serial.print("Pressure mmHg  "); Serial.println(pressure_mmHg);
  //Serial.print("calibration number  "); Serial.println(calibration_offset);
  return pressure_mmHg;
}

void pressure_inflate(int inflate_pressure){
  float current_pressure = pressure_sensor_read();
  while(current_pressure+5<inflate_pressure){
    pump_on();
    current_pressure = pressure_sensor_read();
    Serial.print("Current_Pressure   ");
    Serial.println(current_pressure);
    
  }

  //5 is an offset that sets a deadband around pressure to preven rapid cycling of valve/pump
  while((current_pressure-5)>inflate_pressure){
    pump_off();
    valve_open();
    current_pressure = pressure_sensor_read();
  }
  if((current_pressure-5)<inflate_pressure){
    valve_close();
  }

}

float pressure_find_occlusion(){
  int average_occlusion_pressure_array = 0;
  if (occlusion_start_flag = true){
    pressure_inflate(inflate_pressure_value);
    Serial.print("occlusion start flag pre check  ");
    Serial.println(find_occlusion_pressure_flag);
    occlusion_start_flag = false;
    Serial.print("current pressure   ");
    Serial.println(pressure_sensor_read());
    Serial.print("Current pressure inflation value  ");
    Serial.println(inflate_pressure_value);
    Serial.print("Occlusion start flag  ");
    Serial.println(occlusion_start_flag);
    
  }

  occlusion_start_flag = false;

  while(!occlusion_stop_flag){
    Serial.println("Occlusion Array Math");
    inflate_pressure_value = inflate_pressure_value+10;
    for(int i=0;i<201;i++){
    occlusion_pressure_array[i] = pressure_sensor_read();
    delay(10);
    }
    for(int i=0; i<(occlusion_array_size+1);i++){
      sum_occlusion_array=sum_occlusion_array + occlusion_pressure_array[i];
    }
    average_occlusion_pressure_array = sum_occlusion_array/occlusion_array_size;
    for (int i = 0; i < occlusion_array_size;i++ ){
		  if ( occlusion_pressure_array[i] > max_occlusion_array_value ){
			  max_occlusion_array_value = occlusion_pressure_array[i];
		  }
	  }

    Serial.print("Max Pressure   ");
    Serial.println(max_occlusion_array_value);
    Serial.print("Average Pressure  ");
    Serial.println(average_occlusion_pressure_array);
    occlusion_array_difference = max_occlusion_array_value - average_occlusion_pressure_array;
    Serial.print("Occlusion Array Difference  ");
    Serial.println(occlusion_array_difference);
    Serial.print("Arterial Compression Flag  ");
    Serial.println(arterial_compression_flag);
    Serial.print("Occlusion Array Difference < Setpoint");
    Serial.println(occlusion_array_difference > occlusion_array_difference_setpoint);

    if ((occlusion_array_difference > occlusion_array_difference_setpoint)){
        arterial_compression_flag = true;
        Serial.println("Arterial Compression");
    }

    if ((occlusion_array_difference < occlusion_array_difference_setpoint)){
        Serial.println("Occlusion");
        //occlusion_stop_flag = true;
        Serial.print("Occlusion stop flag   ");
        //Serial.println(occlusion_stop_flag);
    }

  }

  found_occlusion_pressure = pressure_sensor_read();
  Serial.print("Occlusion Pressure  ");
  Serial.println(found_occlusion_pressure);
  return found_occlusion_pressure;
}

void pressure_inflate_percentage_occlusion(int percentage_occlusion_setpoint){
  if(!find_occlusion_pressure_flag){
    occlusion_pressure = pressure_find_occlusion();
    find_occlusion_pressure_flag=true;
  }
  float percent_occlusion_setpoint_multiplier = (0.01* percentage_occlusion_setpoint);
  pressure_inflate(percent_occlusion_setpoint_multiplier*occlusion_pressure);

}