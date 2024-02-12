#include "HallSensor.h"
#include "DisplayManager.h"
#include "SpeedControl.h"

#include "ODriveUART.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

// Definiere die GPIO-Pins für die analogen Sensoren
const int HALL_BW_PIN = 35; // GPIO35 = HALL2 Backward
const int HALL_FW_PIN = 34; // GPIO34 = HALL1 Forward 

// Definiere den Wertebereich der von den Pedalen an oDrive übergeben wird
const int MAX_HALL_VALUE  = 100; // Der maximale Wert der zurückgegeben wird wenn Pedale voll durchgedrückt werden
const int MIN_HALL_VALUE = 0; // Der minimale Wert der zurückgegeben wird wenn Pedale nicht gedrückt werden
const int MAX_KMH_MODE_R = 5; // Maximalgeschwindigkeit in KM/h beim Rückwertsfahren --> (Errechnet sich aus Reifendurchmesser RADIUSCM)
const int MAX_KMH_MODE_1 = 5; // Maximalgeschwindigkeit in KM/h im Modus 1 --> (Errechnet sich aus Reifendurchmesser RADIUSCM)
const int MAX_KMH_MODE_2 = 10; // Maximalgeschwindigkeit in KM/h im Modus 2 --> (Errechnet sich aus Reifendurchmesser RADIUSCM) 
const int MAX_KMH_MODE_3 = 18;// Maximalgeschwindigkeit in KM/h im Modus 3 --> (Errechnet sich aus Reifendurchmesser RADIUSCM)
const int MAX_KMH_MODE_4 = 30; // Maximalgeschwindigkeit in KM/h im Modus 4 --> (Errechnet sich aus Reifendurchmesser RADIUSCM) 

// Definiere Reifendurchmesser (Für Geschwindigkeitberechnung) in cm
const float RADIUSCM = 20.5;

// Definiere ESP32 GPIO-Pins und UART-Schnittstelle
const int ODRIVE_BAUD_RATE = 115200; // Baudrate zur Kommunikation mit oDrive
const int ODRIVE_UART = 1; // verwendeter UART-Bus des ESP32
const int ODRIVE_RX = 10; // verwendeter GPIO für RX UART-Bus des ESP32
const int ODRIVE_TX = 9; // verwendeter GPIO für TX UART-Bus des ESP32

// Buttons
#define BUTTON_1 13 // verwendeter GPIO für Button 1
#define BUTTON_2 14 // verwendeter GPIO für Button 2

// bugfix
int countLoop = 0;

HallSensor sensor;  //initialisiere HALL-Sensoren (Gas/Bremse)
DisplayManager displayManager; //initialisiere OLED-Display
SpeedControler speedControler; //initialisiere Geschwinigkeitskontrolle
HardwareSerial odrive_serial(ODRIVE_UART); // Verwender UART im ESP32
ODriveUART odrive(odrive_serial);


int getMode(){
  pinMode(BUTTON_1, INPUT_PULLUP); 
  pinMode(BUTTON_2, INPUT_PULLUP); 
  int buttonOneState = digitalRead(BUTTON_1);
  int buttonTwoState = digitalRead(BUTTON_2);

  if(buttonOneState == LOW && buttonTwoState == LOW){
    return 4;
  }
  else if (buttonTwoState == LOW){
    return 2;
  }
  else if (buttonOneState == LOW){
    return 3;
  }
  else {
    return 1;
  }
}

void setup() {
    Serial.begin(115200); // serieller Monitor
    odrive_serial.begin(ODRIVE_BAUD_RATE, SERIAL_8N1, ODRIVE_RX, ODRIVE_TX); // odrive
    sensor.setup(MAX_HALL_VALUE , MIN_HALL_VALUE); // HALL Sensoren (Pedale)
    displayManager.setup(MAX_HALL_VALUE , MIN_HALL_VALUE); // Display
    speedControler.setup(MAX_HALL_VALUE , MIN_HALL_VALUE, RADIUSCM); // Geschwindigkeit Outputkontrolle
    displayManager.displayMessage("Boote ODrive");
    displayManager.displaySpeedmode(getMode(), true);
    displayManager.displayKMh(23.56,67, getMode());


    delay(10);

  // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive.getState(0) == AXIS_STATE_UNDEFINED) {
      delay(30);
    }

    // setze Axis0 in Closed_loop (externen Steuerungsmodus)
    while (odrive.getState(0) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
      odrive.clearErrors();
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 0);
      delay(10);
    }

      // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive.getState(1) == AXIS_STATE_UNDEFINED) {
      delay(30);
    }

    // setze Axis0 in Closed_loop (externen Steuerungsmodus)
    while (odrive.getState(1) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
      odrive.clearErrors();
      odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 1);
      delay(10);
    }

    Serial.println("ODrive running!");
  }


void loop() {
  countLoop +=  1;
  Serial.println(countLoop);
  odrive_serial.flush();
  Serial.flush();
  odrive.resetWatchdog(0); // Setze Watchdog Axis 0 zurück (odrive geht sonst nach 0.8sek in Notaus)
  odrive.resetWatchdog(1); // Setze Watchdog Axis 0 zurück (odrive geht sonst nach 0.8sek in Notaus)

  // sammle HALL Werte
  int backwardValue = sensor.readValue(HALL_BW_PIN);
  int forwardValue = sensor.readValue(HALL_FW_PIN);
  int inputSpeedValueInt = speedControler.getSpeed(forwardValue, backwardValue, 0);
  float inputSpeedValue = static_cast<float>(inputSpeedValueInt) * 0.001;
  //displayManager.displayValues(backwardValue, forwardValue);

  // sammle odrive Daten
  float vBusVoltage =  odrive.getParameterAsFloat("vbus_voltage");
  float odriveVel =  odrive.getParameterAsFloat("axis0.encoder.vel_estimate");

  float speedKMh = speedControler.speedKMh(odriveVel);


  displayManager.displayMessage(speedKMh);
  // Gesetzte Geschwindigkeit

  //odrive.setVelocity(inputSpeedValue, 0);
  odrive.setVelocity(inputSpeedValue);
  
  Serial.print("KM/H: ");
  Serial.println(speedKMh);

  Serial.print(" : ");
  Serial.println(vBusVoltage);
  
  //delay(0);
}

