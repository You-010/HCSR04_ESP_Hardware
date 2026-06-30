# HCSR04_ESP_Hardware
Hardware-based and non-blocking HC-SR04 ultrasonic library using native ESP32 MCPWM edge capture and FreeRTOS task notifications. Designed for asynchronous measurement alongside Wi-Fi, Bluetooth and other tasks.

---

## Credits & Technical Documentation
This library is based on the hardware edge-capture timestamp concepts demonstrated in the official [Espressif ESP-IDF MCPWM Capture HC-SR04 Example](https://github.com/espressif/esp-idf/tree/v4.4.1/examples/peripherals/mcpwm/mcpwm_capture_hc_sr04), updated to support modern ESP-IDF v5+ drivers.

---

## Framework Support
Supports both Arduino and pure ESP-IDF frameworks.

---

## Hardware Setup
* **VCC:** 5V (for max range) or 3.3V (convenient, but may reduce range/compatibility).
* **GND:** Connect to ESP32 Ground.
* **Trig:** Any GPIO (3.3V logic triggers 5V sensors fine).
* **Echo:** Target hardware input GPIO.
  * *WARNING:* If powering via 5V, use a passive **resistor voltage divider** (e.g. 1kΩ/2kΩ) to step the Echo pulse down to 3.3V. Avoid active transistor level shifters, as they introduce microsecond timing delays that skew readings.

---
## Hardware Compatibility
This library relies entirely on native ESP32 silicon hardware blocks and **will only compile on ESP32 variants that feature hardware MCPWM modules**. 

If using an incompatible board (such as an ESP32 variant that lacks native MCPWM), compilation will stop immediately with an error message.

---

## Quick Start  

### 1. Standalone Sensor Mode
Use this mode to run single sensors on an independent background thread.  
Each sensor will use one MCPWM timer channel.
```cpp
#include <Arduino.h>
#include "HCSR04_ESP.h"

HCSR04Sensor sensor(11, 12); // Trig, Echo

void setup() {
  Serial.begin(115200);
  if (!sensor.begin()) {
    Serial.println(sensor.getLastErrorString());
    while(1);
  }
}

void loop() {
  float distance = sensor.readDistanceCm();    
  Serial.printf("Distance: %.2f cm\n", distance);
  delay(500);
}
```

### 2. Multi-Sensor Controller Mode
Use this mode to multiplex multiple sensors on one MCPWM timer channel, conserving channels.  
Sensors attached to the same controller *may* share their trigger pin to conserve pins.
```cpp
#include <Arduino.h>
#include "HCSR04_ESP.h"

HCSR04Sensor sensor1(11, 12); 
HCSR04Sensor sensor2(11, 13); // Shares Trig pin 11

HCSR04Controller controller; 

void setup() {
  Serial.begin(115200);
  
  controller.addSensor(&sensor1);
  controller.addSensor(&sensor2);
  
  if (!controller.begin()) {
    Serial.println(controller.getLastErrorString());
    while(1);
  }
}

void loop() {
  float d1 = sensor1.readDistanceCm();
  float d2 = sensor2.readDistanceCm();
  Serial.printf("S1: %.1f cm | S2: %.1f cm\n", d1, d2);
  delay(500);
}
```

### Output Config Flags

Customise `readDistanceCm(mode)` by combining one flag from **Group A** and one from **Group B** with a bitwise OR (`|`) (e.g., `MEDIAN | RAW`).

#### Group A: Sampling Modes
* `HCSR04Sensor::MEDIAN` (Default) – Takes multiple readings and returns the median to filter out noise.
* `HCSR04Sensor::SINGLE` – Returns a single, immediate measurement.

#### Group B: Validation Modes
* `HCSR04Sensor::VALID` (Default) – Restricts outputs to `minCm`/`maxCm` bounds; returns `-1.0f` on failure.
* `HCSR04Sensor::RAW` – Bypasses boundary checks to return the raw value.

#### Quick Examples:
```cpp
sensor.readDistanceCm();               // (MEDIAN | VALID)  Default
sensor.readDistanceCm(RAW);            // (MEDIAN | RAW)    Median filtered + unbounded
sensor.readDistanceCm(SINGLE);         // (SINGLE | VALID)  Single read + validated
sensor.readDistanceCm(SINGLE | RAW);   // (SINGLE | RAW)    Single read + unbounded

```
