#ifndef SpeedControler_h
#define SpeedControler_h
#include <cmath> // Für M_PI
#include <Arduino.h>
#include "Constants.h"
#include "ODriveUART.h"


class SpeedController {
  public:
    SpeedController() : odrive(nullptr){} // setze Standardwerte


    void setSpeedMode(SpeedMode mode) {
        current_speed_mode = mode;
    }

    SpeedMode getSpeedMode() {
        return current_speed_mode;
    }

    void setControlMode(ControlMode mode) {
        current_control_mode = mode;
    }

    ControlMode getControlMode() {
        return current_control_mode;
    }

    void setVelocityGain(float gain) {
        velocity_gain = gain;
        // Anwenden auf ODrive falls notwendig
    }

    float getVelocityGain() {
        return velocity_gain;
    }

    void setVelocityIntegratorGain(float gain) {
        velocity_integrator_gain = gain;
        // Anwenden auf ODrive falls notwendig
    }

    float getVelocityIntegratorGain() {
        return velocity_integrator_gain;
    }


    void setODrive(ODriveUART* odriveInstance) {
        odrive = odriveInstance;
    }

    float getVBusCurrent() {
      float amper = odrive->getParameterAsFloat("axis0.motor.current_control.Iq_measured") + odrive->getParameterAsFloat("axis1.motor.current_control.Iq_measured"); // aktuelle amp
      return amper;
    }

    // ODrive initialisieren/setup -> Ändert die Konfig dauerhaft
    void initializeODrive() {
        if (odrive != nullptr) {
            odrive->clearErrors();
            odrive->setVelocityGain(velocity_gain); // Proportionaler Anteil p-PID
            odrive->setVelocityIntegratorGain(velocity_integrator_gain);  // Integratoranteil i-PID

            if (current_control_mode == VELOCITY_CONTROL) {
                odrive->setControlMode("CONTROL_MODE_VELOCITY_CONTROL");
            } else if (current_control_mode == VOLTAGE_CONTROL) {
                odrive->setControlMode("CONTROL_MODE_VOLTAGE_CONTROL");
            }
            else {
                odrive->setControlMode("CONTROL_MODE_TORQUE_CONTROL");
            }

            odrive->setParameter("axis0.motor.config.current_lim", BAT_MAX_CURRENT/2);
            odrive->setParameter("axis1.motor.config.current_lim", BAT_MAX_CURRENT/2);

            odrive->setParameter("axis0.motor.config.current_lim_margin", BAT_MAX_CURRENT_MARGIN/2);
            odrive->setParameter("axis1.motor.config.current_lim_margin", BAT_MAX_CURRENT_MARGIN/2);

            odrive->saveConfig();
            odrive->reboot();
        }
    }

    // Alle Bewegungen stoppen
    void stopAll() {
        if (odrive != nullptr) {
            odrive->setState(AXIS_STATE_IDLE, 0);
            odrive->setState(AXIS_STATE_IDLE, 1);
        }
    }

    bool hardwareHealthCheck(){
        // Batterie check
        while(odrive->getParameterAsFloat("vbus_voltage") == 0.0){
            delay(30);
        }
        float vBat = odrive->getParameterAsFloat("vbus_voltage");
        if(vBat <= V_BAT_MIN_START) {
            // displayManager.vBatLowError(vBat); // Endlosschleife -> Batterie leer
        }
        // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
        while (odrive->getState(0) == AXIS_STATE_UNDEFINED) {
            delay(30);
        }
        // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
        while (odrive->getState(1) == AXIS_STATE_UNDEFINED) {
            delay(30);
        }

        // setze Axis0 in Closed_loop (externen Steuerungsmodus)
        while (odrive->getState(0) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
            odrive->setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 0);
            delay(10);
        }

        // setze Axis1 in Closed_loop (externen Steuerungsmodus)
        while (odrive->getState(1) != AXIS_STATE_CLOSED_LOOP_CONTROL) { 
            odrive->setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 1);
            delay(10);
        }

        odrive->setParameter("axis0.config.enable_watchdog", true);
        odrive->setParameter("axis1.config.enable_watchdog", true);

    }
    

    // Idle-Kontrolle
    void idleControl(float odrive_kmh, float request_kmh) {
        if (odrive != nullptr) {
            request_kmh = abs(request_kmh);
            if (request_kmh == 0 && odrive_kmh == 0) {
                if (odrive->getState(0) != AXIS_STATE_IDLE) {
                    odrive->setState(AXIS_STATE_IDLE, 0);
                    odrive->setState(AXIS_STATE_IDLE, 1);
                    delay(30);
                }
            }
            else if(request_kmh >= 0.2) {
                if (odrive->getState(0) != AXIS_STATE_CLOSED_LOOP_CONTROL) {
                    odrive->setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 0);
                    odrive->setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 1);
                    delay(30);
                }
            }
        }
    }

    // Fehlerkontrolle
    int getErrors(ODriveErrors* errors) {
            int errorCount = 0;

            if (odrive != nullptr) {
                int systemError = odrive->getParameterAsInt("error");
                if (systemError != 0) {
                    errors[errorCount].errorCode = systemError;
                    errors[errorCount].source = "System";
                    errorCount++;
                }

                int axis0Error = odrive->getParameterAsInt("axis0.error");
                if (axis0Error != 0) {
                    errors[errorCount].errorCode = axis0Error;
                    errors[errorCount].source = "Axis0";
                    errorCount++;
                }

                int axis1Error = odrive->getParameterAsInt("axis1.error");
                if (axis1Error != 0) {
                    errors[errorCount].errorCode = axis1Error;
                    errors[errorCount].source = "Axis1";
                    errorCount++;
                }
            }

            return errorCount;
        }

    // Aktuelle Geschwindigkeit auslesen in RPS
    float getCurrentVelocity() {
        if (odrive != nullptr) {
            float vel_axis0 = odrive->getParameterAsFloat("axis0.encoder.vel_estimate");
            float vel_axis1 = odrive->getParameterAsFloat("axis1.encoder.vel_estimate");
            return (vel_axis0 + vel_axis1) / 2.0; // Durchschnittsgeschwindigkeit der beiden Achsen
        }
        return 0.0;
    }

    // Zielgeschwindigkeit setzen
    void setTargetVelocity(float velocity) {
        if (odrive != nullptr) {
            odrive->setVelocity(velocity);
        }
    }

    float getRequestedRPS() {
        SpeedModusParameter mode = modiParameter[current_speed_mode];
        int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
        float maxRPS = convertKMHtoRPS(mode.maxSpeed);

        float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxRPS*100); 
        return toReturn/100; 
    }

    float getRequestedNm() {
        SpeedModusParameter mode = modiParameter[current_speed_mode];
        int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
        float maxNM = mode.maxTorque;

        float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxNM*100); 
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
    SpeedMode current_speed_mode; // Aktuell ausgewählter Geschwindigkeitsmodus
    ControlMode current_control_mode; // Aktuell ausgewählter SteuerungsModus
    ODriveUART* odrive; // Zeiger auf die ODrive Instanz
    float velocity_gain; // p-PID
    float velocity_integrator_gain; // i-PID
};
#endif
