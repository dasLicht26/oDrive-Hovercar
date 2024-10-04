#include "Config.h"
#include "DisplayManager.h"
#include "SpeedController.h"
#include "EEPROMSettings.h"
#include "ODriveUART.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

DisplayManager displayManager; //initialisiere Display-Steuerung
SpeedController speedController; //initialisiere Geschwinigkeitskontrolle
EepromSettings eepromSettings; // initialisiere Manager für dauerhaft gespeicherte Einstellungen
HardwareSerial odrive_serial(ODRIVE_UART); // Verwendeter UART im ESP32
ODriveUART odrive(odrive_serial);

std::vector<ODriveErrors> odrive_errors; // Fehlerliste
int errorCount; // Anzahl der Fehler

// Knopfzustände
bool button_ok; 
bool button_up;
bool button_down;

void setup() {
   
    // Setup SerialBus-Controller
    Serial.begin(115200); // starte seriellen Monitor zu ESP32
    odrive_serial.begin(ODRIVE_BAUD_RATE, SERIAL_8N1, ODRIVE_RX, ODRIVE_TX); // starte serielle Verbindung zum odrive
    displayManager.setup(); // Initialisiere DisplayManager
    eepromSettings.setup(); // Initialisiere EPROM Speicher
    speedController.setODrive(&odrive); // übergebe Zeiger auf ODrive-Objekt an SpeedController
    speedController.setEeprom(&eepromSettings); // übergebe Zeiger auf EepromSettings-Objekt an SpeedController

    // Initialisiere HardwareButtons
    pinMode(BUTTON_UP, INPUT_PULLUP); 
    pinMode(BUTTON_DOWN, INPUT_PULLUP); 
    pinMode(BUTTON_OK, INPUT_PULLUP); 

    speedController.setWatchdogEnabled(false);

    // Check ob oDrive ready ist und Batterie ausreichend geladen 
    speedController.hardwareStartUpCheck();

    Serial.print("asdw");
    // Setze Geschwindigkeitsmodus 
    speedController.calculateSpeedMode();
    Serial.print("33");
    // aktiviere Watchdog
    speedController.setWatchdogEnabled(true);

    // Setze DisplayManager auf Mainmenü
    displayManager.setSpeedController(&speedController); // übergebe Zeiger auf SpeedController-Objekt an DisplayManager

    // checke ob Debug-Modus aktiviert wird
    if(!digitalRead(BUTTON_OK) && !digitalRead(BUTTON_DOWN)){
      DEBUG_MODE_AKIV = true;
    }
}  


void loop() {

  speedController.resetWatchdog();
  // Knopfstatus lesen
  button_ok = !digitalRead(BUTTON_OK);
  button_up = !digitalRead(BUTTON_UP);
  button_down = !digitalRead(BUTTON_DOWN);

  ///Test
  // Überprüfe, ob Daten über den Serial Monitor empfangen wurden
  if (Serial.available() > 0 && LOCAL_DEBUG) {
      char input = Serial.read(); // Lese die Eingabe

      // Setze die Button-Zustände basierend auf der Eingabe
      button_ok = (input == '1');
      button_up = (input == '2');
      button_down = (input == '3');

  }

  
  // Lese Knopfinput und aktualisiere Menü
  displayManager.handleInput(button_ok, button_up, button_down);

  // Lese Fehlerliste von ODrive --> Wenn Fehler vorhanden, dann blockiere den Loop
  if (!LOCAL_DEBUG){
    odrive_errors = speedController.getErrors();
    errorCount = odrive_errors.size();
    if(errorCount > 0){
      displayManager.setMenuState(ERROR_ODRIVE);
      speedController.stopCar();
      speedController.stopMotorControl();

      // Blockiert den Loop, bis der Fehler behoben wurde.
      while (true) {
        displayManager.updateDisplay();
        delay(1000);
      }
    }
  }
  



  // Lese Batteriespannung --> Wenn Batterie leer, dann blockiere den Loop
  if(speedController.isBatteryLow() && !LOCAL_DEBUG){
    displayManager.setMenuState(ERROR_LOW_VOLTAGE);
    speedController.stopCar();
    speedController.stopMotorControl();

    // Blockiert den Loop.
    while (true) {
      displayManager.updateDisplay();
    }
  }
  if(!LOCAL_DEBUG){
    speedController.updateSpeed(); // Aktualisiere Geschwindigkeit und das aktuelle Verhalten, je nach Input und Modus
  }
  displayManager.updateDisplay(); // Aktualisiere Menü und Display

  // gib input und angeforderten speed aus
  Serial.println(speedController.getRequestedRPS());

                    
}


