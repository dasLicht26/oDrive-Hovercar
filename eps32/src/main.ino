#include "Config.h"
#include "DisplayManager.h"
#include "SpeedController.h"
#include "EEPROMSettings.h"

#include "ODriveUART.h"
#include <Arduino.h>
#include <SoftwareSerial.h>


DisplayManager displayManager; //initialisiere OLED-Display
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

    displayManager.setup(); // Initialisiere Display
    eepromSettings.setup(); // Initialisiere EPROM Speicher

    // Initialisiere HardwareButtons
    pinMode(BUTTON_UP, INPUT_PULLUP); 
    pinMode(BUTTON_DOWN, INPUT_PULLUP); 
    pinMode(BUTTON_OK, INPUT_PULLUP); 


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

  // Menü aktualisieren
  displayManager.updateMenu(buttonOK, buttonUP, buttonDOWN, speedController, odrive);
  
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

  speedController.updateSpeed(); // Aktualisiere Geschwindigkeit

  /*
  // setze odrive Watchdog-Timer zurück, wenn dieser nicht alle 0.8 Sekunden zurückgesetzt wird, gehen die Motoren in Notaus (Falls es ein Verbindungsabbruch gibt)
  odrive.resetWatchdog(0); // Setze Watchdog Axis 0 zurück 
  //odrive.resetWatchdog(1); // Setze Watchdog Axis 0 zurück 

  // lese Batteriespannung
  float vBat =  odrive.getParameterAsFloat("vbus_voltage");
  int vBatPercent = map(vBat, V_BAT_MIN, V_BAT_MAX, 0, 100);

  // checke ob Batteriespannung i.O. ist
    if(vBat <= V_BAT_MIN) {
      odrive.setVelocity(0.0); // Bremse auf 0
      displayManager.vBatLowError(vBat); // Endlosschleife -> Batterie leer
    return; // Breche den Loop ab
  }

  // lese aktuelle Geschwindigkeit der angeforderten und tatsächlichen KMH/RPS
  float odriveRPS0 =  odrive.getParameterAsFloat("axis0.encoder.vel_estimate");
  float odriveRPS1 =  odrive.getParameterAsFloat("axis1.encoder.vel_estimate");
  float odriveRPS = (odriveRPS1 + odriveRPS0)/2; // Geschwindigkeitsmittel der zwei Räder
  float odriveKMH = speedController.convertRPStoKMh(odriveRPS);
 
  float amp = odrive.getParameterAsFloat("axis0.motor.current_control.Iq_measured") + odrive.getParameterAsFloat("axis1.motor.current_control.Iq_measured"); // aktuelle amp
  float nm = amp * odrive.getParameterAsFloat("axis0.motor.config.torque_constant"); // nm
  
  
  float requestRPS = speedController.getRequestedRPS();
  float requestNM = speedController.getRequestedNm();

  float maxRps = speedController.convertKMHtoRPS(maxKmh);

  float requestKMH = speedController.convertRPStoKMh(requestRPS);







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


