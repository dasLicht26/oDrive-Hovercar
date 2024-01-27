#include "HallSensor.h"
#include "DisplayManager.h"

AnalogSensor sensor;
DisplayManager displayManager;

void setup() {
  Serial.begin(115200);
  sensor.setup();
  displayManager.setup();
}

void loop() {
  int sensorValue1 = sensor.readValue(A0); // Ersetze A0 durch den tatsächlichen Pin
  int sensorValue2 = sensor.readValue(A1); // Ersetze A1 durch den tatsächlichen Pin
  displayManager.displayValues(sensorValue1, sensorValue2);
  delay(500);
}