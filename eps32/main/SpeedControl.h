#ifndef SpeedControler_h
#define SpeedControler_h
#include <cmath> // Für M_PI
#include <Arduino.h>
#include "Constants.h"


const int HALL_RESOLUTION = 100; // 

class SpeedController {
  public:
    SpeedController() : currentModus(MODUS_1) {} // Initialisiere mit Modus 1

    // setze SpeedModus (MODUS_1, MODUS_...)
    void setSpeedModus(SpeedModus modus) {
        currentSpeedModus = modus;
      }

    void setControlMode(ControlMode cmodus){
        currentControlModus = cmodus;
      }

    float getRequestedRPS() {
      int hallValue = getHallMappedValue(forwartGPIO) - getHallMappedValue(backwartGPIO);
      float maxRPS;
      if (hallValue < 0){
        maxRPS = convertKMHtoRPS(maxKmhBW);
      }
      else {
        maxRPS = convertKMHtoRPS(maxKmhFW);
      }
      float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxRPS*100); 

      return toReturn/100; 
    }

    float getRequestedNm() {
      int hallValue = getHallMappedValue(forwartGPIO) - getHallMappedValue(backwartGPIO);
      float maxRPS;
      if (hallValue < 0){
        maxRPS = convertKMHtoRPS(maxKmhBW);
      }
      else {
        maxRPS = convertKMHtoRPS(maxKmhFW);
      }
      float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxRPS*100); 

      return toReturn/100; 
    }


  float convertRPStoKMh(float RPS) {
    float radiusM = radiusCm / 100.0; // Konvertiere cm in m
    float circumferenceM  = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = circumferenceM * RPS; // m/s
    return speedMS * 3.6 *-1; // Konvertiere m/s in km/h
  }

  float convertKMHtoRPS(float KMH) {
    float radiusM = radiusCm / 100.0; // Konvertiere cm in m
    float circumferenceM = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = KMH / 3.6; // Konvertiere km/h in m/s
    return speedMS / circumferenceM; // Berechne RPS
  }

  private:
    SpeedModus currentSpeedModus; // Aktuell ausgewählter Geschwindigkeitsmodus

    // Gibt den Hallinput als Wert zwischen 0 und 100 zurück
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
};

#endif