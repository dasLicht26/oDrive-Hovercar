#include "HallSensor.h"
#include "DisplayManager.h"
#include "SpeedControl.h"

// Definiere die GPIO-Pins für die analogen Sensoren
const int analogPin1 = 35; // GPIO35 = HALL2 Backward
const int analogPin2 = 34; // GPIO34 = HALL1 Forward 

// Definiere den Wertebereich der von den Pedalen an oDrive übergeben wird
const int maxSpeedValue = 10; // GPIO35 = HALL2 Backward
const int minSpeedValue = 0; // GPIO34 = HALL1 Forward 

HallSensor sensor;
DisplayManager displayManager;
SpeedControler speedControler;


void setup() {
  Serial.begin(115200);
  sensor.setup(maxSpeedValue, minSpeedValue);
  displayManager.setup(maxSpeedValue, minSpeedValue);
  speedControler.setup(maxSpeedValue, minSpeedValue);
}

void loop() {
  int backwardValue = sensor.readValue(analogPin1);
  int forwardValue = sensor.readValue(analogPin2);
  int speedValue = speedControler.getSpeed(forwardValue, backwardValue, 0);
  displayManager.displayValues(backwardValue, forwardValue);

  
  // Ausgabe auf der Konsole
  Serial.print("Geschwindigkeit: ");
  Serial.println(speedValue);




  delay(50);
}

