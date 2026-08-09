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

      const uint32_t settingsVersionExpected = 0x48435632; // "HCV2"
      uint32_t settingsVersion;
      EEPROM.get(28, settingsVersion);
      if (settingsVersion != settingsVersionExpected) {
        // Einmalige Migration von den alten Drehmomentwerten auf sichere Fahrwerte.
        EEPROM.put(0, STANDARD_SETTING_ITEMS[3].current_value);
        EEPROM.put(4, STANDARD_SETTING_ITEMS[2].current_value);
        EEPROM.put(20, STANDARD_SETTING_ITEMS[0].current_value);
        EEPROM.put(24, STANDARD_SETTING_ITEMS[1].current_value);
        EEPROM.put(28, settingsVersionExpected);
        EEPROM.commit();
      }
    }

    void saveVelocityGain(float gain) {
      EEPROM.put(20, gain);
      EEPROM.commit();
    }

    float loadVelocityGain() {
      float gain;
      EEPROM.get(20, gain);
      if (isnan(gain) || gain < 0.0f || gain > 10.0f) {
        gain = STANDARD_SETTING_ITEMS[0].current_value;
        saveVelocityGain(gain);
      }
      return gain;
    }

    void saveVelocityIntegratorGain(float gain) {
      EEPROM.put(24, gain);
      EEPROM.commit();
    }

    float loadVelocityIntegratorGain() {
      float gain;
      EEPROM.get(24, gain);
      if (isnan(gain) || gain < 0.0f || gain > 50.0f) {
        gain = STANDARD_SETTING_ITEMS[1].current_value;
        saveVelocityIntegratorGain(gain);
      }
      return gain;
    }

    void saveBrakeRate(float brake_rate) {
      EEPROM.put(0, brake_rate); // Speichert es an Pos. 0
      EEPROM.commit(); // Stelle sicher, dass die Daten in den EEPROM geschrieben werden
    }

    float loadBrakeRate() {
      float brake_rate;
      EEPROM.get(0, brake_rate); // Lese aus Pos. 0
      // Verwirft auch den alten Nm-Slope-Wert aus frueheren Firmware-Versionen.
      if (isnan(brake_rate) || brake_rate < 0.5f || brake_rate > 20.0f) {
        brake_rate = STANDARD_SETTING_ITEMS[3].current_value;
        saveBrakeRate(brake_rate);
      }
      return brake_rate;
    }

    void saveAccelerationRate(float acceleration_rate) {
      EEPROM.put(4, acceleration_rate); // Speichert es an Pos. 4
      EEPROM.commit(); // Stelle sicher, dass die Daten in den EEPROM geschrieben werden
    }

    float loadAccelerationRate() {
      float acceleration_rate;
      EEPROM.get(4, acceleration_rate); // Lese aus Pos. 4
      if (isnan(acceleration_rate) || acceleration_rate < 0.5f || acceleration_rate > 20.0f) {
        acceleration_rate = STANDARD_SETTING_ITEMS[2].current_value;
        saveAccelerationRate(acceleration_rate);
      }
      return acceleration_rate;
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
