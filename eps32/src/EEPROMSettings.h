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

    void saveTorqueSlope(float nm_slope) {
      EEPROM.put(0, nm_slope); //Speichert es an Pos. 0
      EEPROM.commit(); // Stelle sicher, dass die Daten in den EEPROM geschrieben werden
    }

    float loadTorqueSlope() {
      float nm_slope;
      EEPROM.get(0, nm_slope); // Lese aus Pos. 0
      if (isnan(nm_slope)) {
        nm_slope = STANDARD_SETTING_ITEMS[3].current_value;
        saveTorqueSlope(nm_slope);
      }
      return nm_slope;
    }

    void saveTorqueMinimum(float nm_minimum) {
      EEPROM.put(4, nm_minimum); //Speichert es an Pos. 4
      EEPROM.commit(); // Stelle sicher, dass die Daten in den EEPROM geschrieben werden
    }

    float loadTorqueMinimum() {
      float nm_minimum;
      EEPROM.get(4, nm_minimum); // Lese aus Pos. 4

      // Wenn Wert nicht gesetzt ist oder ungültig (nan) ist, setze ihn auf 2.0
      if (isnan(nm_minimum)) {
        nm_minimum = STANDARD_SETTING_ITEMS[2].current_value;
        saveTorqueMinimum(nm_minimum);
      }

      return nm_minimum;
    }

};

#endif