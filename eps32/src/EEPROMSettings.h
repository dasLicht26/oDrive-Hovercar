#ifndef EEPROMSettings_h
#define EEPROMSettings_h

#include <EEPROM.h>
#include "SpeedController.h"
#include "config.h"

struct Settings {
    SpeedMode speedMode;
    ControlMode controlMode;
    float velocityGain;
    float velocityIntegratorGain;
};

class EepromSettings {
  public:
    void setup(){
      EEPROM.begin(512); // Initialisiere EEPROM mit 512 Bytes
    }

    void saveSettings(const Settings& settings) {
      EEPROM.put(0, settings); //Speichert es an Pos. 0
      EEPROM.commit(); // Stelle sicher, dass die Daten in den EEPROM geschrieben werden
    }

    void loadSettings(Settings& settings) {
      EEPROM.get(0, settings); // Lese aus Pos. 0
        if (isValidSettings(settings)) {
            // Geladene Einstellungen sind valide
        } else {
            // Lade Standardwerte, falls die gespeicherten Einstellungen ungültig sind
            settings.speedMode = MODE_1;
            settings.controlMode = VELOCITY_CONTROL;
            settings.velocityGain = 0.9; // Setze deine Standardwerte
            settings.velocityIntegratorGain = 3.0; // Setze deine Standardwerte
        }
    }

  private:
    bool isValidSettings(const Settings& settings) {
      return  !isnan(settings.velocityGain) && settings.velocityGain >= 0.0 &&
              !isnan(settings.velocityIntegratorGain) && settings.velocityIntegratorGain >= 0.0;
    }
};

#endif