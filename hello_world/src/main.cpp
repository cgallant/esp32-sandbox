#include <Arduino.h>

#define LED_PIN 38

void setup() {
  Serial.begin(115200);
  delay(2000);  // Wait 2 seconds for serial to connect
  Serial.println("ESP32 Started!");
  pinMode(LED_PIN, OUTPUT);
  neopixelWrite(LED_PIN, 0, 0, 0);
}

void loop() {
  Serial.println("Red");
  neopixelWrite(LED_PIN, 50, 0, 0);
  delay(1000);
  
  Serial.println("Green");
  neopixelWrite(LED_PIN, 0, 50, 0);
  delay(1000);
  
  Serial.println("Blue");
  neopixelWrite(LED_PIN, 0, 0, 50);
  delay(1000);
}