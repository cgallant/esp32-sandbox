#include <Arduino.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("MCP23017 One-Chip Button Test");

  if (!mcp1.begin_I2C(0x20)) {
    Serial.println("MCP23017 #1 (0x20) not found!");
    while (1);
  }
  Serial.println("MCP23017 #1 (0x20) found!");

  // Configure GPA0-GPA7 as inputs with pull-ups on both chips
  for (int i = 0; i < 8; i++) {
    mcp1.pinMode(i, INPUT_PULLUP);
    mcp2.pinMode(i, INPUT_PULLUP);
  }
}

void loop() {
  Serial.print("Chip1: ");
  for (int i = 6; i < 8; i++) {
    Serial.print(mcp1.digitalRead(i));
    Serial.print(" ");

  }
  Serial.println();
  //delay(200);
}