#ifndef SpeedControler_h
#define SpeedControler_h
#include <cmath> // Für M_PI
#include <Arduino.h>

const int HALL_RESOLUTION = 100; // 

class SpeedControler {
  public:
    void setup(int _maxAnalogeValue, int _minAnalogeValue, int _backwartGPIO, int _forwartGPIO, int _maxKmhFW, int _maxKmhBW, float _radiusCm) {
      maxAnalogeValue = _maxAnalogeValue; // max analoger Input 
      minAnalogeValue = _minAnalogeValue; // min anlaoger Input
      backwartGPIO = _backwartGPIO; // GPIO Rückwärts-Pedale
      forwartGPIO = _forwartGPIO; // GPIO Vorwärts-Pedale
      maxKmhFW = _maxKmhFW; // maximal erlaubte Geschwindigkeit Vorwärts
      maxKmhBW = _maxKmhBW; // maximal erlaubte Geschwindigkeit Rückwerts
      radiusCm = _radiusCm; // Reifendurchmesser
    }

    int getHallMappedValue(int gpio) {
      int rawValue = analogRead(gpio);
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

    float getRequestedRPS() {
      int speedValue = getHallMappedValue(forwartGPIO) - getHallMappedValue(backwartGPIO);
      float maxRPS;
      if (speedValue < 0){
        maxRPS = convertKMHtoRPS(maxKmhBW);
      }
      else {
        maxRPS = convertKMHtoRPS(maxKmhFW);
      }
      float toReturn = map(speedValue, 0, HALL_RESOLUTION, 0, maxRPS*100); 

      return toReturn/100; 
    }


  float convertRPStoKMh(float RPS) {
    float radiusM = radiusCm / 100.0; // Konvertiere cm in m
    float circumferenceM  = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = circumferenceM * RPS; // m/s
    return speedMS * 3.6; // Konvertiere m/s in km/h
  }

  float convertKMHtoRPS(float KMH) {
    float radiusM = radiusCm / 100.0; // Konvertiere cm in m
    float circumferenceM = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = KMH / 3.6; // Konvertiere km/h in m/s
    return speedMS / circumferenceM; // Berechne RPS
  }

  private:
    int maxAnalogeValue; // max analoger Input
    int minAnalogeValue; // min analoger Input
    int backwartGPIO;    // GPIO Rückwärts-Pedale
    int forwartGPIO;     // GPIO Vorwärts-Pedale
    int maxKmhFW;        // maximal erlaubte Geschwindigkeit Vorwärts in Km/h
    int maxKmhBW;        // maximal erlaubte Geschwindigkeit Rückwärts in Km/h
    float radiusCm;      // Reifendurchmesser
};

#endif