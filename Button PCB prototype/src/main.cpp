/*******************************************************************************
 * @file    main.cpp
 * @author  [Your Name]
 * @date    2025-12-21
 * @brief   MCP23017 Quad-Chip Button Test
 *
 * @description
 * Reads button inputs from four MCP23017 I/O expanders over I2C.
 * - MCP23017 #1 at address 0x20: reads GPA6-GPA7
 * - MCP23017 #2 at address 0x21: reads GPA6-GPA7
 * - MCP23017 #3 at address 0x22: reads GPA6-GPA7
 * - MCP23017 #4 at address 0x23: reads GPA6-GPA7
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
 * 2025-12-21 - Added MCP23017 #3 and #4 at addresses 0x22 and 0x23
 ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

const int NUM_MCPS = 4;
const uint8_t MCP_ADDRESSES[] = {0x20, 0x21, 0x22, 0x23};

Adafruit_MCP23X17 mcp[NUM_MCPS];

void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("MCP23017 Quad-Chip Button Test");

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
}

void loop() {
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