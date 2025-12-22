/*******************************************************************************
 * @file    main.cpp
 * @author  [Your Name]
 * @date    2025-12-21
 * @brief   MCP23017 Dual-Chip Button Test
 *
 * @description
 * Reads button inputs from two MCP23017 I/O expanders over I2C.
 * - MCP23017 #1 at address 0x20: reads GPA6-GPA7
 * - MCP23017 #2 at address 0x21: reads GPA6-GPA7
 *
 * @hardware
 * - ESP32-S3-DevKitC-1
 * - I2C on GPIO 8 (SDA) and GPIO 9 (SCL)
 * - Buttons wired to GPA6, GPA7 on each MCP23017 (active low with pull-ups)
 *
 * @dependencies
 * - Adafruit MCP23017 Arduino Library v2.3.2
 *
 * @revision
 * 2025-12-21 - Initial version with single MCP23017
 * 2025-12-21 - Added second MCP23017 at address 0x21
 ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("MCP23017 Dual-Chip Button Test");

  Wire.begin(8, 9);
  delay(100);  // Let I2C bus settle

  // Initialize MCP23017 #1 at address 0x20
  if (!mcp1.begin_I2C(0x20, &Wire)) {
    Serial.println("MCP23017 #1 (0x20) not found!");
    while (1);
  }
  Serial.println("MCP23017 #1 (0x20) found!");

  // Initialize MCP23017 #2 at address 0x21
  if (!mcp2.begin_I2C(0x21, &Wire)) {
    Serial.println("MCP23017 #2 (0x21) not found!");
    while (1);
  }
  Serial.println("MCP23017 #2 (0x21) found!");

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

  Serial.print("  Chip2: ");
  for (int i = 6; i < 8; i++) {
    Serial.print(mcp2.digitalRead(i));
    Serial.print(" ");
  }

  Serial.println();
}