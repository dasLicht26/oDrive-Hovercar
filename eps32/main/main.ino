#include "HallSensor.h"
#include "DisplayManager.h"
#include "SpeedControl.h"
#include "ODriveInterface.h"

// Definiere die GPIO-Pins für die analogen Sensoren
const int HALL_BW_PIN = 35; // GPIO35 = HALL2 Backward
const int HALL_FW_PIN = 34; // GPIO34 = HALL1 Forward 

// Definiere den Wertebereich der von den Pedalen an oDrive übergeben wird
const int MAX_SPEED_VALUE  = 10; // Der maximale Wert der zurückgegeben wird wenn Pedale voll durchgedrückt werden
const int MIN_SPEED_VALUE = 0; // Der minimale Wert der zurückgegeben wird wenn Pedale nicht gedrückt werden

// Definiere oDrive Pins und UART-Schnittstelle
const int RX_PIN = 10; //UART-1 GPIO10 RX --> TX oDrive
const int TX_PIN = 9;  //UART-1 GPIO09 TX --> RX oDrive
const int ODRIVE_BAUD_RATE = 115200; // Baudrate zur Kommunikation mit oDrive
const int ODRIVE_UART = 1; // verwendeter UART-Bus des ESP32

HallSensor sensor;  //initialisiere HALL-Sensoren (Gas/Bremse)
DisplayManager displayManager; //initialisiere OLED-Display
SpeedControler speedControler; //initialisiere Geschwinigkeitskontrolle
HardwareSerial odrive_serial(ODRIVE_UART); // Verwende UART
ODriveInterface odrive(odrive_serial); //initialisiere Odrive

void setup() {
  Serial.begin(115200); // serieller Monitor
  odrive.begin(BAUD_RATE, RX_PIN, TX_PIN); // oDrive steuerung
  sensor.setup(MAX_SPEED_VALUE , MIN_SPEED_VALUE);
  displayManager.setup(MAX_SPEED_VALUE , MIN_SPEED_VALUE);
  speedControler.setup(MAX_SPEED_VALUE , MIN_SPEED_VALUE);
}

void loop() {
  int backwardValue = sensor.readValue(HALL_BW_PIN);
  int forwardValue = sensor.readValue(HALL_FW_PIN);
  int speedValue = speedControler.getSpeed(forwardValue, backwardValue, 0);
  displayManager.displayValues(backwardValue, forwardValue);

  // Echte Geschwindigeit
  float velocity = odrive.readVelocity();
  Serial.print("ODrive Geschwindigkeit: ");
  Serial.println(velocity);
  
  // Gesetzte Geschwindigkeit
  Serial.print("Geschwindigkeit: ");
  Serial.println(speedValue);


  delay(50);
}

