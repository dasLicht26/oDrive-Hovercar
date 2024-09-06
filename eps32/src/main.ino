#include "DisplayManager.h"
#include "SpeedController.h"
#include "Constants.h"
#include "ConfigManager.h"

#include "ODriveUART.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

String speedMode; // Gesetzter Geschwindigkeitesmodus -> Wert wird beim Einschalten/Booten gesetzt (setSpeedMode())
int maxKmh; // Gesetzte Maximalgeschwindigkeit in Km/h -> Wert wird beim Einschalten/Booten gesetzt (setSpeedMode())
bool idleMode = false; // ob sich odive im idlemode befindet 

DisplayManager displayManager; //initialisiere OLED-Display
SpeedController speedController; //initialisiere Geschwinigkeitskontrolle
ConfigManager configManager; // initialisiere Manager für dauerhaft gespeicherte Einstellungen
HardwareSerial odrive_serial(ODRIVE_UART); // Verwendeter UART im ESP32
ODriveUART odrive(odrive_serial);

MenuState currentMenuState = MENU_MAIN; // standard Menüzustand

std::vector<ODriveErrors> odrive_errors; // Fehlerliste
int errorCount; // Anzahl der Fehler

// Knopfzustände
bool buttonOK; 
bool buttonUP;
bool buttonDOWN;

bool debug_mode = false; // ob der DebugModus aktiv ist

bool idle_break = false; // ob im stillstehenden Zustand gebremst wird

float bat_voltage; // Batteriespannung

void setup() {
   
    // Setup SerialBus-Controller
    Serial.begin(115200); // starte seriellen Monitor zu ESP32
    odrive_serial.begin(ODRIVE_BAUD_RATE, SERIAL_8N1, ODRIVE_RX, ODRIVE_TX); // starte serielle Verbindung zum odrive

    // Initialisiere Display
    displayManager.setup();

    configManager.setup();

    // Initialisiere HardwareButtons
    pinMode(BUTTON_UP, INPUT_PULLUP); 
    pinMode(BUTTON_DOWN, INPUT_PULLUP); 
    pinMode(BUTTON_OK, INPUT_PULLUP); 

    // Einstellungen laden
    Settings settings;
    configManager.loadSettings(settings);

    // Geladene Einstellungen anwenden
    Serial.println(settings.velocityGain);
    speedController.setODrive(&odrive);
    speedController.setSpeedMode(settings.speedMode);
    speedController.setControlMode(settings.controlMode);
    speedController.setVelocityGain(settings.velocityGain);
    speedController.setVelocityIntegratorGain(settings.velocityIntegratorGain);
    //speedController.saveODriveConfig();

    // checke nach Debug-Modus
    buttonUP = !digitalRead(BUTTON_UP);
    buttonDOWN = !digitalRead(BUTTON_DOWN);
    if (buttonDOWN && !buttonUP) {
      debug_mode = true;
    };   
}

void loop() {
  // Knopfstatus lesen
  buttonOK = !digitalRead(BUTTON_OK);
  buttonUP = !digitalRead(BUTTON_UP);
  buttonDOWN = !digitalRead(BUTTON_DOWN);

  // Menü aktualisieren (nur im Debug-Modus)
  if (debug_mode){
    displayManager.updateMenu(buttonOK, buttonUP, buttonDOWN, speedController, configManager, odrive);
  };

  // Lese Fehlerliste von ODrive
  odrive_errors = speedController.getErrors();
  errorCount = odrive_errors.size();
  if(errorCount > 0){
    displayManager.setMenuState(ERROR_ODRIVE);
    speedController.stopAll();

    // Blockiert den Loop, bis der Fehler behoben wurde.
    while (true) {
      delay(1000);
    }
  }

  // setze Geschwindigkeit
  speedController.updateEngine(idle_break);


  // lese Batteriespannung
  float bat_voltage =  odrive.getParameterAsFloat("vbus_voltage");

  // checke ob Batteriespannung i.O. ist
  if(bat_voltage <= V_BAT_MIN) {
    odrive.setVelocity(0.0); // Bremse auf 0
    displayManager.setMenuState(ERROR_LOW_VOLTAGE); // Endlosschleife -> Batterie leer
    // Blockiert den Loop, bis die Batterie wieder aufgeladen wurde.
    while (true) {
      delay(1000);
    }
  }
           
}


