/*******************************************************************************
 * @file    main.cpp
 * @author  [Your Name]
 * @date    2025-12-21
 * @brief   MCP23017 Seven-Chip Button Test with TLC5947 LED Driver
 *
 * @description
 * Reads button inputs from seven MCP23017 I/O expanders over I2C.
 * Controls LEDs via TLC5947 based on button presses.
 * - MCP23017 #1-7 at addresses 0x20-0x26: reads GPA6-GPA7
 * - TLC5947: LED 3 = Chip1 GPA7, LED 4 = Chip1 GPA6
 *
 * @hardware
 * - ESP32-S3-DevKitC-1
 * - I2C on GPIO 8 (SDA) and GPIO 9 (SCL)
 * - TLC5947: LAT=GPIO15, CLK=GPIO38, DIN=GPIO39
 * - Buttons wired to GPA6, GPA7 on each MCP23017 (active low with pull-ups)
 *
 * @dependencies
 * - Adafruit MCP23017 Arduino Library v2.3.2
 * - Adafruit TLC5947 v1.2.3
 *
 * @revision
 * 2025-12-21 - Initial version with single MCP23017
 * 2025-12-21 - Added second MCP23017 at address 0x21
 * 2025-12-21 - Added MCP23017 #3 and #4 at addresses 0x22 and 0x23
 * 2025-12-21 - Added MCP23017 #5 and #6 at addresses 0x24 and 0x25
 * 2025-12-21 - Added MCP23017 #7 at address 0x26
 * 2025-12-21 - Added TLC5947 LED driver
 ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_TLC5947.h>

// MCP23017 setup
const int NUM_MCPS = 7;
const uint8_t MCP_ADDRESSES[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26};
Adafruit_MCP23X17 mcp[NUM_MCPS];

// TLC5947 setup
#define TLC_LAT   15
#define TLC_CLK   38
#define TLC_DIN   39
#define NUM_TLC   1

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC, TLC_CLK, TLC_DIN, TLC_LAT);

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("MCP23017 Seven-Chip Button Test with TLC5947");

  Wire.begin(8, 9);
  delay(100);  // Let I2C bus settle

  // Initialize all MCP23017 chips
  for (int chip = 0; chip < NUM_MCPS; chip++) {
    if (!mcp[chip].begin_I2C(MCP_ADDRESSES[chip], &Wire)) {
      Serial.printf("MCP23017 #%d (0x%02X) not found!\n", chip + 1, MCP_ADDRESSES[chip]);
      while (1);
    }
    Serial.printf("MCP23017 #%d (0x%02X) found!\n", chip + 1, MCP_ADDRESSES[chip]);

    // Configure GPA0-GPA7 as inputs with pull-ups
    for (int i = 0; i < 8; i++) {
      mcp[chip].pinMode(i, INPUT_PULLUP);
    }
  }

  // Initialize TLC5947
  tlc.begin();
  for (int i = 0; i < 24 * NUM_TLC; i++) {
    tlc.setPWM(i, 0);
  }
  tlc.write();
  Serial.println("TLC5947 initialized!");
}

void loop() {
  // Read buttons on Chip 1
  bool gpa7_pressed = !mcp[0].digitalRead(7);  // Active low
  bool gpa6_pressed = !mcp[0].digitalRead(6);

  // Control LEDs
  tlc.setPWM(3, gpa7_pressed ? 4095 : 0);
  tlc.setPWM(4, gpa6_pressed ? 4095 : 0);
  tlc.write();

  // Print all chip states
  for (int chip = 0; chip < NUM_MCPS; chip++) {
    Serial.printf("Chip%d: ", chip + 1);
    for (int i = 6; i < 8; i++) {
      Serial.print(mcp[chip].digitalRead(i));
      Serial.print(" ");
    }
    Serial.print(" ");
  }
  Serial.println();
}