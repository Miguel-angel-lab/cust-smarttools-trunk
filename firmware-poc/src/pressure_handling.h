#ifndef pressure_handling_h
#define pressure_handling_h

void pressure_sensor_init();
void pressure_sensor_calibrate();
float pressure_sensor_read();
void pressure_inflate(int inflate_pressure);
float pressure_find_occlusion();
void pressure_inflate_percentage_occlusion(int percentage_occlusion_setpoint);


#endif