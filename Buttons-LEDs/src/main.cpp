#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_TLC5947.h>

// MCP23017 setup
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

// TLC5947 setup
#define DATA_PIN   10
#define CLK_PIN    11
#define LATCH_PIN  12
#define NUM_TLC5947 1

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5947, CLK_PIN, DATA_PIN, LATCH_PIN);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("MCP23017 Buttons + TLC5947 LEDs Test");

  // Initialize MCP23017 #1
  if (!mcp1.begin_I2C(0x20)) {
    Serial.println("MCP23017 #1 (0x20) not found!");
    while (1);
  }
  Serial.println("MCP23017 #1 (0x20) found!");

  // Initialize MCP23017 #2
  if (!mcp2.begin_I2C(0x21)) {
    Serial.println("MCP23017 #2 (0x21) not found!");
    while (1);
  }
  Serial.println("MCP23017 #2 (0x21) found!");

  // Configure GPA0-GPA7 as inputs with pull-ups on both chips
  for (int i = 0; i < 8; i++) {
    mcp1.pinMode(i, INPUT_PULLUP);
    mcp2.pinMode(i, INPUT_PULLUP);
  }

  // Initialize TLC5947
  tlc.begin();
  
  // Turn all LEDs off
  for (int i = 0; i < 24; i++) {
    tlc.setPWM(i, 0);
  }
  tlc.write();
  
  Serial.println("Ready! Press buttons GPA0-GPA3 on Chip 1");
}

void loop() {
  // Read buttons GPA0-GPA3 on chip 1 and control LEDs 0-3
  for (int i = 0; i < 4; i++) {
    bool buttonPressed = !mcp1.digitalRead(i);  // LOW when pressed (pull-up)
    
    if (buttonPressed) {
      tlc.setPWM(i, 4095);  // LED on at full brightness
    } else {
      tlc.setPWM(i, 0);     // LED off
    }
  }
  tlc.write();
  
  delay(10);  // Small delay for stability
}