#include <Arduino.h>
#include "HCSR04_ESP.h"

// HC-SR04 connected to GPIO 11 (Trig) and GPIO 12 (Echo)
const uint8_t HCSR04_TRIG_PIN = 11;
const uint8_t HCSR04_ECHO_PIN = 12;
HCSR04Sensor sensor(HCSR04_TRIG_PIN, HCSR04_ECHO_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("HCSR04 sensor starting");
  if (!sensor.begin()) {
    Serial.printf("Failed to initialize hardware! Error: %s\n", sensor.getLastErrorString());
    while(1) delay(1000);
  }
  Serial.println("HCSR04 sensor ready");
}

void loop() {
  // 1. Standard read: uses median-filtering and range validation 
  float distance_v = sensor.readDistanceCm();    
  Serial.print("VALID: "); Serial.print(distance_v); 
           
  // 2. Raw read: Bypasses validation check
  float distance_r = sensor.readDistanceCm(HCSR04Sensor::RAW);
  Serial.print("cm, RAW: "); Serial.print(distance_r);

  // 3. Single raw read: Bypasses median-filtering and validation check
  float distance_s = sensor.readDistanceCm(HCSR04Sensor::SINGLE | HCSR04Sensor::RAW);
  Serial.print("cm, RAW: "); Serial.print(distance_s);

  // 4. temperature adjustment
  float distance_h = sensor.readDistanceCm(40.0f);    //temperature in Celcius
  Serial.print("cm, HOT: "); Serial.print(distance_h); Serial.println("cm");
  
  delay(500);
}
