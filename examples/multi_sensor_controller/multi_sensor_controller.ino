#include <Arduino.h>
#include "HCSR04_ESP.h"

// define pin configurations for three unique sensors
// sensors on the same controller may share trigger pin
const uint8_t sensor1_pins[2] = {11,12};
const uint8_t sensor2_pins[2] = {11,13};
const uint8_t sensor3_pins[2] = {14,15};

// create individual sensor objects
HCSR04Sensor sensor1(sensor1_pins[0], sensor1_pins[1]);
HCSR04Sensor sensor2(sensor2_pins[0], sensor2_pins[1]);
HCSR04Sensor sensor3(sensor3_pins[0], sensor3_pins[1]);

// create controller, you can pass the delay between sensor bursts in ms (default 60ms if blank)
HCSR04Controller controller(100); 

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("HCSR04 controller starting");

  // attach sensors to the controller 
  if (!controller.addSensor(&sensor1) ||
      !controller.addSensor(&sensor2) ||
      !controller.addSensor(&sensor3)) {
    Serial.printf("Failed to add sensor! Error: %s\n", controller.getLastErrorString());
    while (1) delay(1000);
  }

  // begin controller
  // pass 'true' to fail immediately if any trigger pins conflict with other controllers or standalone sensors
  // default is false, the confliting sensor will be skipped, begin() returns true but getLastError() still flags error
  if (!controller.begin(true)) {
    Serial.printf("Failed to initialize hardware! Error: %s\n", controller.getLastErrorString());
    while (1) delay(1000);
  }
  Serial.println("HCSR04 controller ready");
}

void loop() {
  // read distance from sensors asynchronously
  // the controller automatically updates these in the background
  float dist1 = sensor1.readDistanceCm();
  float dist2 = sensor2.readDistanceCm();
  float dist3 = sensor3.readDistanceCm();

  // print results
  Serial.print("sensor1: ");
  if (dist1 >= 0.0f) Serial.print(dist1, 1); else Serial.print("---");
  Serial.print("cm | sensor2: ");
  if (dist2 >= 0.0f) Serial.print(dist2, 1); else Serial.print("---");
  Serial.print("cm | sensor3: ");
  if (dist3 >= 0.0f) Serial.print(dist3, 1); else Serial.print("---");
  Serial.println("cm");

  delay(500);
}
