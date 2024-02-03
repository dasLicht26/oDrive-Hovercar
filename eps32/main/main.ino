#include "HallSensor.h"
#include "DisplayManager.h"
#include "SpeedControl.h"
//#include "ODriveInterface.h"
#include <ODriveUART.h>

// Definiere die GPIO-Pins für die analogen Sensoren
const int HALL_BW_PIN = 35; // GPIO35 = HALL2 Backward
const int HALL_FW_PIN = 34; // GPIO34 = HALL1 Forward 

// Definiere den Wertebereich der von den Pedalen an oDrive übergeben wird
const int MAX_SPEED_VALUE  = 10; // Der maximale Wert der zurückgegeben wird wenn Pedale voll durchgedrückt werden
const int MIN_SPEED_VALUE = 0; // Der minimale Wert der zurückgegeben wird wenn Pedale nicht gedrückt werden

// Definiere oDrive Pins und UART-Schnittstelle
const int RX_PIN = 9; //UART-1 GPIO10 RX --> TX oDrive (GPIO 01)
const int TX_PIN = 10;  //UART-1 GPIO09 TX --> RX oDrive (GPIO 02)
const int ODRIVE_BAUD_RATE = 115200; // Baudrate zur Kommunikation mit oDrive
const int ODRIVE_UART = 1; // verwendeter UART-Bus des ESP32

HallSensor sensor;  //initialisiere HALL-Sensoren (Gas/Bremse)
DisplayManager displayManager; //initialisiere OLED-Display
SpeedControler speedControler; //initialisiere Geschwinigkeitskontrolle
HardwareSerial odrive_serial(ODRIVE_UART); // Verwende UART
//ODriveInterface odrive(odrive_serial); //initialisiere Odrive
//SoftwareSerial odrive_serial(8, 9);
ODriveUART odrive(odrive_serial);

void setup() {
  Serial.begin(115200); // serieller Monitor
  //odrive.begin(ODRIVE_BAUD_RATE, RX_PIN, TX_PIN); // oDrive steuerung
  odrive_serial.begin(ODRIVE_BAUD_RATE);

  sensor.setup(MAX_SPEED_VALUE , MIN_SPEED_VALUE);
  displayManager.setup(MAX_SPEED_VALUE , MIN_SPEED_VALUE);
  speedControler.setup(MAX_SPEED_VALUE , MIN_SPEED_VALUE);
  delay(10);
  while (odrive.getState() == AXIS_STATE_UNDEFINED) {
    delay(100);
  }
  while (odrive.getState() != AXIS_STATE_CLOSED_LOOP_CONTROL) {
    odrive.clearErrors();
    odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL);
    delay(10);
  }
  Serial.println("ODrive running!");
}

void loop() {
  int backwardValue = sensor.readValue(HALL_BW_PIN);
  int forwardValue = sensor.readValue(HALL_FW_PIN);
  int speedValue = speedControler.getSpeed(forwardValue, backwardValue, 0);
  float speedValueFloat = static_cast<float>(speedValue) * 0.1;
  displayManager.displayValues(backwardValue, forwardValue);
  //odrive_serial.flush(); // Leere den Eingangspuffer
  // Echte Geschwindigeit
  //float velocity = odrive.readVelocity();
  //float vbat = odrive.readBatteryVoltage();
  //float current = odrive.readCurrent();

  //Serial.print("ODrive Geschwindigkeit: ");
  //Serial.println(velocity);

  //Serial.print("ODrive Volt: ");
  //Serial.println(vbat);
  
  //Serial.print("ODrive Ampere: ");
  //Serial.println(current);

  // Gesetzte Geschwindigkeit
  Serial.print("Geschwindigkeit: ");
  Serial.println(speedValueFloat);
  ODriveFeedback feedback = odrive.getFeedback();
  Serial.print("pos:");
  Serial.print(feedback.pos);
  Serial.print(", ");
  Serial.print("vel:");
  Serial.print(feedback.vel);
  Serial.println();
  odrive.setVelocity(speedValueFloat);
  //odrive.writeVelocity(speedValueFloat);

}

