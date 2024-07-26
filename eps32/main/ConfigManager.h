#ifndef ConfigManager_h
#define ConfigManager_h

#include <EEPROM.h>
#include "SpeedController.h"
#include "Constants.h"

class ConfigManager {
  public:
    void setup(){
      EEPROM.begin(512); // Initialisiere EEPROM mit 512 Bytes
    }

    void saveSettings(const Settings& settings) {
      EEPROM.put(0, settings);
      EEPROM.commit(); // Stelle sicher, dass die Daten in den EEPROM geschrieben werden
    }

    void loadSettings(Settings& settings) {
      EEPROM.get(0, settings);
        if (isValidSettings(settings)) {
            // Geladene Einstellungen sind valide
        } else {
            // Lade Standardwerte, falls die gespeicherten Einstellungen ungültig sind
            settings.speedMode = MODE_1;
            settings.controlMode = VELOCITY_CONTROL;
            settings.velocityGain = 3.0; // Setze deine Standardwerte
            settings.velocityIntegratorGain = 1.0; // Setze deine Standardwerte
        }
    }

    void save(int test) {
      EEPROM.write(4, test);
      EEPROM.commit(); // Stelle sicher, dass die Daten in den EEPROM geschrieben werden
    }
    void conf(){
      EEPROM.begin(512);
    }

    int load() {
      int test;
      test = EEPROM.read(4);
      return test;
    }
  private:
    bool isValidSettings(const Settings& settings) {
      return  !isnan(settings.velocityGain) && settings.velocityGain >= 0.0 &&
              !isnan(settings.velocityIntegratorGain) && settings.velocityIntegratorGain >= 0.0;
    }
};

#endif