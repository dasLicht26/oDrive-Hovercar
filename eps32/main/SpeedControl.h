#ifndef SpeedControler_h
#define SpeedControler_h
#include <cmath> // Für M_PI
#include <Arduino.h>
#include "Constants.h"



class SpeedController {
  public:
    SpeedController() : currentSpeedMode(MODE_1), currentControlMode(TORQUE_CONTROL) {} //setzte Standartwerte

    // setze SpeedModus (MODUS_1, MODUS_...)
    void setSpeedMode(SpeedMode mode) {
        currentSpeedMode = mode;
      }

    // setze ControllMode (SPEED_CONTROL, TORQUE_CONTROL)
    void setControlMode(ControlMode mode){
        currentControlMode = mode;
      }

    float getRequestedRPS() {
      SpeedModusParameter mode = modiParameter[currentSpeedMode];
      int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
      float maxRPS;

      maxRPS = convertKMHtoRPS(mode.maxSpeed);

      float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxRPS*100); 

      return toReturn/100; 
    }

    float getRequestedNm() {
      SpeedModusParameter mode = modiParameter[currentSpeedMode];
      int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
      float maxRPS;

      maxRPS = convertKMHtoRPS(mode.maxSpeed);

      float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxRPS*100); 

      return toReturn/100; 
    }


  float convertRPStoKMh(float RPS) {
    float radiusM = RADIUSCM / 100.0; // Konvertiere cm in m
    float circumferenceM  = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = circumferenceM * RPS; // m/s
    return speedMS * 3.6 *-1; // Konvertiere m/s in km/h
  }

  float convertKMHtoRPS(float KMH) {
    float radiusM = RADIUSCM / 100.0; // Konvertiere cm in m
    float circumferenceM = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = KMH / 3.6; // Konvertiere km/h in m/s
    return speedMS / circumferenceM; // Berechne RPS
  }

  // Gibt den Hallinput als Wert zwischen 0 und HALL_RESOLUTION (100) zurück
  int getHallMappedValue(int gpio) {
      int rawValue = analogRead(gpio);
      int toReturn = map(rawValue, HALL_ANALOG_MIN, HALL_ANALOG_MAX, 0, HALL_RESOLUTION); // 0.5V und 3.0V auf 0-100 Skala mappen

      // verhinden, dass Werte unter 0 oder über HALL_RESOLUTION zurückgegeben werden
      if(toReturn <= 0){
        toReturn = 0;
      }
      if(toReturn >= HALL_RESOLUTION){
        toReturn = HALL_RESOLUTION;
      }
      return toReturn; 
    }

  private:
    SpeedMode currentSpeedMode; // Aktuell ausgewählter Geschwindigkeitsmodus
    ControlMode currentControlMode; // Aktuell ausgewählter SteuerungsModus
};
#endif