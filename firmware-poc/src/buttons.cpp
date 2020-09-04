#include <Arduino.h>
#include <EasyButton.h>
#include "buttons.h"

#define button_pin_1 21
#define button_pin_2 4

EasyButton button(button_pin_1);

int double_click_spreed = 750;
bool stop_flag = false;

void buttonPressed()
{
  Serial.println("Button Pressed");
}

void sequenceEllapsed()
{
  Serial.println("Double click");

}



void buttonISR()
{
  //When button is being used through external interrupts, parameter INTERRUPT must be passed to read() function
  button.read(INTERRUPT);
}

void buttons_init(){
    button.begin();
  button.onPressed(buttonPressed);
  button.onSequence(2, double_click_spreed, sequenceEllapsed);
  if (button.supportsInterrupt())
  {
    button.enableInterrupt(buttonISR);
    Serial.println("Button will be used through interrupts");
  }
}