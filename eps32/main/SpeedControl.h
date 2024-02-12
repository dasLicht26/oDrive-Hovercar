#ifndef SpeedControler_h
#define SpeedControler_h
#include <cmath> // Für M_PI
#include <Arduino.h>

const int HALL_RESOLUTION = 100 // 

class SpeedControler {
  public:
    void setup(int _maxAnalogeValue, int _minAnalogeValue, int HALL_BW_PIN, int HALL_FW_PIN, float _radiusCm) {
      maxAnalogeValue = _maxAnalogeValue;
      minAnalogeValue = _minAnalogeValue;
      radiusCm = _radiusCm;
    }

    int getHallValue(int pin) {
      int rawValue = analogRead(pin);
      int toReturn = map(rawValue, minAnalogeValue, maxAnalogeValue, 0, HALL_RESOLUTION); // 0.5V und 3.0V auf 0-100 Skala mappen

      // verhinden, dass Werte unter 0 oder über HALL_RESOLUTION zurückgegeben werden
      if(toReturn <= 0){
        toReturn = 0;
      }
      if(toReturn >= HALL_RESOLUTION){
        toReturn = HALL_RESOLUTION;
      }
      return toReturn; 
    }

    int getRPS(int forwartValue, int backwardValue, int currentSpeed) {
      int speedValue = forwartValue - backwardValue;
      return speedValue; 
    }

  float getKMh( float odriveRPS) {
    float radiusM = radiusCm / 100.0; // Konvertiere cm in m
    float circumferenceM  = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = circumferenceM * odriveRPS; // m/s
    return speedMS * 3.6; // Konvertiere m/s in km/h
  }

  private:
    float radiusCm;
    int maxValue;
    int minValue;
};

#endif