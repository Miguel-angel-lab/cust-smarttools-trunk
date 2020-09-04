#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1325.h>

#define OLED_CLK 27
#define OLED_MOSI 33

// These are neede for both hardware & softare SPI
#define OLED_CS 15
#define OLED_RESET 32
#define OLED_DC 12

Adafruit_SSD1325 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void display_init(){
    display.begin(); 
    display.clearDisplay();
    display.setTextSize(1.5);
    display.setCursor(25, 25);
    display.setTextColor(WHITE);
    display.print("Smart Tools");
    display.display();
}

void display_pressure(float pressure){
    int display_pressure = pressure;
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(0, 16);
    display.setTextColor(WHITE);
    display.print(display_pressure);
    display.setTextSize(2);
    display.setCursor(75,25);
    display.print("mmHg");
    display.display();
}

