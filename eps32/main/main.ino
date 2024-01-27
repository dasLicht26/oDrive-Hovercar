#include "HallSensor.h"
#include "DisplayManager.h"

// Definiere die GPIO-Pins für die analogen Sensoren
const int analogPin1 = 33; // GPIO33
const int analogPin2 = 34; // GPIO34

HallSensor sensor;
DisplayManager displayManager;

void setup() {
  Serial.begin(115200);
  sensor.setup();
  displayManager.setup();
}

void loop() {
  int sensorValue1 = sensor.readValue(analogPin1);
  int sensorValue2 = sensor.readValue(analogPin2);
  displayManager.displayValues(sensorValue1, sensorValue2);
  delay(500);
}