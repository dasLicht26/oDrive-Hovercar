#ifndef SpeedControler_h
#define SpeedControler_h
#include <cmath> // Für M_PI
#include <Arduino.h>
#include "Config.h"
#include "ODriveUART.h"
#include <vector>
#include <string>


class SpeedController {
  public:
    SpeedController() : odrive(nullptr), current_speed_mode(STANDARD_SPEED_MODE),current_control_mode(STANDARD_CONTROL_MODE){} // setze Standardwerte
  
    void setSpeedMode(SpeedMode mode) { current_speed_mode = mode;}

    // Gibt den aktuellen Geschwindigkeitsmodus zurück
    SpeedMode getSpeedMode() { return current_speed_mode;}

    // Gibt die Parameter des aktuellen Geschwindigkeitsmodus zurück
    SpeedModeParameter getSpeedModeParameter() { return modiParameter[current_speed_mode]; }

    // Setze den Steuerungsmodus
    void setControlMode(ControlMode mode) { current_control_mode = mode; }

    // Gibt den aktuellen Steuerungsmodus zurück
    ControlMode getControlMode() { return current_control_mode; }

    // Setze den p-PID Gain
    void setVelocityGain(float gain) { velocity_gain = gain; }

    // Gibt die aktuelle Batteriespannung zurück
    float getBatteryVoltage() { return odrive->getParameterAsFloat("vbus_voltage"); }

    // Gibt den Batterieprozentsatz zurück
    int getBatteryPercentage() { return map(getBatteryVoltage(), V_BAT_MIN, V_BAT_MAX, 0, 100); }

    // Gibt an ob Spannungs Untergrenze erreicht ist
    bool isBatteryLow() { return getBatteryVoltage() < V_BAT_MIN; }

    // Gibt den aktuellen p-PID Gain zurück
    float getVelocityGain() { return velocity_gain; }

    // Setze den Integrator Gain
    void setVelocityIntegratorGain(float gain) { velocity_integrator_gain = gain;}

    // Gibt den aktuellen i-PID Integrator Gain zurück
    float getVelocityIntegratorGain() { return velocity_integrator_gain; }

    // ODrive Instanz setzen
    void setODrive(ODriveUART* odriveInstance) { odrive = odriveInstance; }

    // Aktuelle Batterielast auslesen in Ampere
    float getVBusCurrent();

    // Geschwindigkeit aktualisieren
    void updateSpeed();

    // ODrive initialisieren/setup -> Ändert die oDrive Konfig dauerhaft
    void saveODriveConfig();
    
    // Motorsteuerung deaktivieren -> Setzt den Motor in den Idle-Modus
    void stopMotorControl();

    // Motorsteuerung aktivieren -> Setzt den Motor in den Closed-Loop-Modus
    void startMotorControl();

    // setzte automatisch Vorwärts/Rückwerts Modus (MODE_R wird nur bei Stillstand aktiviert)
    void updateDirectionMode();

    // Bringe das Auto zum Stillstand -> Bremse bis Stillstand, Alle Eingaben (Gas/Bremse) sind bis absoluten Stillstand deaktiviert.
    void stopCar();

    // Starte Hardware und überprüfe ob alles korrekt ist (z.B. ODrive verbunden, Batterie geladen, Leerer Fehlerspeicher, etc)
    void hardwareStartUpCheck();

    // Fehlerkontrolle, liefert eine Listen/struct mit Fehlern zurück
    std::vector<ODriveErrors> getErrors();

    // Aktuelle Geschwindigkeit auslesen in RPS
    float getCurrentVelocity();

    // Zielgeschwindigkeit setzen in RPS
    void setTargetVelocity(float velocity);

    // Zielgeschwindigkeit auslesen in RPS aus Pedalen
    float getRequestedRPS();

    // gibt die aktuelle angeforderte Geschwindigkeit in km/h zurück
    float getRequestedKMH(){ return convertRPStoKMh(getRequestedRPS()); }

    // gibt die aktuelle Geschwindigkeit in km/h zurück
    float getCurrentKMH(){return convertRPStoKMh(getCurrentVelocity()); }

    // gibt das aktuelle angeforderte Drehmoment in Nm zurück
    float getRequestedNm();

    // gibt das aktuelle Drehmoment in Nm zurück
    float getCurrentNM();

    // Konvertiere RPS in km/h
    float convertRPStoKMh(float RPS);

    // Konvertiere km/h in RPS
    float convertKMHtoRPS(float KMH);

    // Gibt den Hallinput als Wert zwischen 0 und HALL_RESOLUTION (100) zurück
    int getHallMappedValue(int gpio);

  private:
    SpeedMode current_speed_mode; // Aktuell ausgewählter Geschwindigkeitsmodus
    SpeedMode temp_speed_mode; // Temporärer Geschwindigkeitsmodus -> Wird für Rückwertsgang benötigt
    ControlMode current_control_mode; // Aktuell ausgewählter SteuerungsModus
    ODriveUART* odrive; // Zeiger auf die ODrive Instanz
    float velocity_gain; // p-PID
    float velocity_integrator_gain; // i-PID
};
#endif
