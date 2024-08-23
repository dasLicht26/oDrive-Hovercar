#include "Config.h"
#include "DisplayManager.h"
#include "SpeedController.h"
#include "EEPROMSettings.h"
#include <Adafruit_SSD1306.h>
#include "ODriveUART.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //initialisiere OLED-Display
DisplayManager displayManager(display); //initialisiere Display-Steuerung
SpeedController speedController; //initialisiere Geschwinigkeitskontrolle
EepromSettings eepromSettings; // initialisiere Manager für dauerhaft gespeicherte Einstellungen
HardwareSerial odrive_serial(ODRIVE_UART); // Verwendeter UART im ESP32
ODriveUART odrive(odrive_serial);

std::vector<ODriveErrors> odrive_errors; // Fehlerliste
int errorCount; // Anzahl der Fehler

// Knopfzustände
bool buttonOK; 
bool buttonUP;
bool buttonDOWN;

void setup() {
   
    // Setup SerialBus-Controller
    Serial.begin(115200); // starte seriellen Monitor zu ESP32
    odrive_serial.begin(ODRIVE_BAUD_RATE, SERIAL_8N1, ODRIVE_RX, ODRIVE_TX); // starte serielle Verbindung zum odrive

    displayManager.displayBootlogo(); // Initialisiere Display
    eepromSettings.setup(); // Initialisiere EPROM Speicher
    speedController.setODrive(&odrive); // übergebe Zeiger auf ODrive-Objekt an SpeedController
    displayManager.setSpeedController(&speedController); // übergebe Zeiger auf SpeedController-Objekt an DisplayManager

    // Initialisiere HardwareButtons
    pinMode(BUTTON_UP, INPUT_PULLUP); 
    pinMode(BUTTON_DOWN, INPUT_PULLUP); 
    pinMode(BUTTON_OK, INPUT_PULLUP); 

    // Check ob oDrive ready ist und Batterie ausreichend geladen 
    speedController.hardwareStartUpCheck();

    // checke ob Debug-Modus aktiviert wird
    if(!digitalRead(BUTTON_OK) && !digitalRead(BUTTON_DOWN)){
      DEBUG_MODE_AKIV = true;
    }
  }   

void loop() {
  // Knopfstatus lesen
  buttonOK = !digitalRead(BUTTON_OK);
  buttonUP = !digitalRead(BUTTON_UP);
  buttonDOWN = !digitalRead(BUTTON_DOWN);

  // Lese Fehlerliste von ODrive --> Wenn Fehler vorhanden, dann blockiere den Loop
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
  
  // Lese Batteriespannung --> Wenn Batterie leer, dann blockiere den Loop
  if(speedController.isBatteryLow()){
    displayManager.setMenuState(ERROR_LOW_VOLTAGE);
    speedController.stopCar();
    speedController.stopMotorControl();

    // Blockiert den Loop, bis die Batterie wieder geladen wurde.
    while (true) {
      displayManager.updateDisplay();
    }
  }

  speedController.updateSpeed(); // Aktualisiere Geschwindigkeit und das aktuelle Verhalten, je nach Input und Modus
  displayManager.updateDisplay(); // Aktualisiere Menü und Display



  

  /*

  Serial.print("current_nm:");
  Serial.println(requestNM);

  //Serial.print("  1:");
  //Serial.print(odrive.getParameterAsString("axis1.motor.config.current_lim_margin"));

  //Serial.print("     vg");

  //Serial.print(odrive.getParameterAsString("axis1.controller.config.vel_gain"));
  //Serial.print("     vig");
  //Serial.println(odrive.getParameterAsString("axis1.controller.config.vel_integrator_gain"));

  odrive.setTorque(requestNM);
  */

  //Serial.print(buttenInputString);                       
  }


