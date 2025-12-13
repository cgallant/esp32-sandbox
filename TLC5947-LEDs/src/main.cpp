#include <Arduino.h>
#include <Adafruit_TLC5947.h>

// TLC5947 pin definitions
#define DATA_PIN   10
#define CLK_PIN    11
#define LATCH_PIN  12
#define NUM_TLC5947 1

Adafruit_TLC5947 tlc = Adafruit_TLC5947(NUM_TLC5947, CLK_PIN, DATA_PIN, LATCH_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("TLC5947 Test Starting...");
  
  tlc.begin();
  
  // Turn all LEDs off first
  for (int i = 0; i < 24; i++) {
    tlc.setPWM(i, 0);
  }
  tlc.write();
  
  Serial.println("All LEDs off. Starting test in 2 seconds...");
  delay(2000);
}

void loop() {
  // Test 1: Turn on LEDs 0-3 one at a time
  Serial.println("Test 1: Sequential on/off");
  for (int i = 0; i < 4; i++) {
    Serial.printf("LED %d ON\n", i);
    tlc.setPWM(i, 4095);  // Full brightness (12-bit max)
    tlc.write();
    delay(500);
    
    tlc.setPWM(i, 0);     // Off
    tlc.write();
    delay(200);
  }
  
  // Test 2: All 4 LEDs on together
  Serial.println("Test 2: All 4 LEDs on");
  for (int i = 0; i < 4; i++) {
    tlc.setPWM(i, 4095);
  }
  tlc.write();
  delay(1000);
  
  // Test 3: Fade all 4 LEDs
  Serial.println("Test 3: Fade up and down");
  for (int brightness = 0; brightness <= 4095; brightness += 100) {
    for (int i = 0; i < 4; i++) {
      tlc.setPWM(i, brightness);
    }
    tlc.write();
    delay(50);
  }
  for (int brightness = 4095; brightness >= 0; brightness -= 100) {
    for (int i = 0; i < 4; i++) {
      tlc.setPWM(i, brightness);
    }
    tlc.write();
    delay(50);
  }
  
  // All off before repeating
  for (int i = 0; i < 4; i++) {
    tlc.setPWM(i, 0);
  }
  tlc.write();
  
  Serial.println("Test complete. Repeating in 2 seconds...\n");
  delay(2000);
}