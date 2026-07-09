#ifndef EEPROMSettings_h
#define EEPROMSettings_h

#include <EEPROM.h>
#include "SpeedController.h"
#include "Config.h"

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

    void saveThrottleCurveExponent(float exponent) {
      EEPROM.put(8, exponent);
      EEPROM.commit();
    }

    float loadThrottleCurveExponent() {
      float exponent;
      EEPROM.get(8, exponent);
      if (isnan(exponent) || exponent < 0.8 || exponent > 3.0) {
        exponent = THROTTLE_CURVE_EXPONENT;
        saveThrottleCurveExponent(exponent);
      }
      return exponent;
    }

    void saveThrottleLinearBlend(float blend) {
      EEPROM.put(12, blend);
      EEPROM.commit();
    }

    float loadThrottleLinearBlend() {
      float blend;
      EEPROM.get(12, blend);
      if (isnan(blend) || blend < 0.0 || blend > 1.0) {
        blend = THROTTLE_LINEAR_BLEND;
        saveThrottleLinearBlend(blend);
      }
      return blend;
    }

    void saveThrottleSmoothingAlpha(float alpha) {
      EEPROM.put(16, alpha);
      EEPROM.commit();
    }

    float loadThrottleSmoothingAlpha() {
      float alpha;
      EEPROM.get(16, alpha);
      if (isnan(alpha) || alpha < 0.05 || alpha > 1.0) {
        alpha = THROTTLE_SMOOTHING_ALPHA;
        saveThrottleSmoothingAlpha(alpha);
      }
      return alpha;
    }

};

#endif
