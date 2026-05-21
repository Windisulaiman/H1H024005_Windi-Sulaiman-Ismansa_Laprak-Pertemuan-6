#include <Arduino.h>

unsigned long previousMillis = 0;   
const long interval = 1000;        
bool ledState = false;              

void setup() {
  pinMode(13, OUTPUT);              
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 
    ledState = !ledState;           
    digitalWrite(13, ledState);     
  }
}
