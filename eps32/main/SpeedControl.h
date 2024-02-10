#ifndef SpeedControler_h
#define SpeedControler_h
#include <cmath> // Für M_PI


class SpeedControler {
  public:
    void setup(int _maxValue, int _minValue, float _radiusCm) {
      maxValue = _maxValue;
      minValue = _minValue;
      radiusCm = _radiusCm;
    }

    int getSpeed(int forwartValue, int backwardValue, int currentSpeed) {
      int speedValue = forwartValue - backwardValue;
      return speedValue; 
    }

  float speedKMh( float oDriveVelocity) {
    float radiusM = radiusCm / 100.0; // Konvertiere cm in m
    float circumferenceM  = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = circumferenceM * oDriveVelocity; // m/s
    return speedMS * 3.6; // Konvertiere m/s in km/h
  }

  private:
    float radiusCm;
    int maxValue;
    int minValue;
};

#endif