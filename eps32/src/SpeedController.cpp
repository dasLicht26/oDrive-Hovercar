#include "SpeedController.h"
#include "Config.h"



// Aktuelle Batterielast auslesen in Ampere
float SpeedController::getVBusCurrent() {
    float amper = odrive->getParameterAsFloat("axis0.motor.current_control.Iq_measured") + odrive->getParameterAsFloat("axis1.motor.current_control.Iq_measured"); // aktuelle amp
    return amper;
}

// Geschwindigkeit aktualisieren
void SpeedController::updateSpeed(){
    float odrive_kmh = getCurrentKMH();
    float request_kmh = getRequestedKMH();
    idleControl(odrive_kmh, request_kmh);
    setTargetVelocity(convertKMHtoRPS(request_kmh));
}

// ODrive initialisieren/setup -> Ändert die Konfig dauerhaft
void SpeedController::saveODriveConfig() {
    if (odrive != nullptr) {
        odrive->clearErrors();
        odrive->setVelocityGain(velocity_gain); // Proportionaler Anteil p-PID
        odrive->setVelocityIntegratorGain(velocity_integrator_gain);  // Integratoranteil i-PID

        switch (getControlMode()){
            case TORQUE_CONTROL: {
                odrive->setParameter("axis0.controller.config.control_mode", "TORQUE_CONTROL");
                odrive->setParameter("axis1.controller.config.control_mode", "TORQUE_CONTROL");
                break;
            }
            case VELOCITY_CONTROL: {
                odrive->setParameter("axis0.controller.config.control_mode", "VELOCITY_CONTROL");
                odrive->setParameter("axis1.controller.config.control_mode", "VELOCITY_CONTROL");
                break;
            }
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
void SpeedController::stopAll() {
    if (odrive != nullptr) {
        odrive->setState(AXIS_STATE_IDLE, 0);
        odrive->setState(AXIS_STATE_IDLE, 1);
    }
}

void SpeedController::hardwareStartUpCheck(){
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
void SpeedController::idleControl(float odrive_kmh, float request_kmh) {
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

// Fehlerkontrolle, liefert eine Listen/struct mit Fehlern zurück
std::vector<ODriveErrors> SpeedController::getErrors() {
    std::vector<ODriveErrors> odrive_errors; // Liste mit fehlern

    if (odrive != nullptr) {
        int systemError = odrive->getParameterAsInt("error");
        if (systemError != 0) {
            ODriveErrors a = {systemError, "System"};
            odrive_errors.push_back(a);
        }

        int axis0Error = odrive->getParameterAsInt("axis0.error");
        if (axis0Error != 0) {
            ODriveErrors b = {axis0Error, "Axis0"};
            odrive_errors.push_back(b);
        }

        int axis1Error = odrive->getParameterAsInt("axis1.error");
        if (axis1Error != 0) {
            ODriveErrors c = {axis1Error, "Axis1"};
            odrive_errors.push_back(c);
        }
    }

    return odrive_errors;
}

// Aktuelle Geschwindigkeit auslesen in RPS
float SpeedController::getCurrentVelocity() {
    if (odrive != nullptr) {
        float vel_axis0 = odrive->getParameterAsFloat("axis0.encoder.vel_estimate");
        float vel_axis1 = odrive->getParameterAsFloat("axis1.encoder.vel_estimate");
        return (vel_axis0 + vel_axis1) / 2.0; // Durchschnittsgeschwindigkeit der beiden Achsen
    }
    return 0.0;
}

// Zielgeschwindigkeit setzen in RPS
void SpeedController::setTargetVelocity(float velocity) {
    if (odrive != nullptr) {
        odrive->setVelocity(velocity);
    }
}

// Zielgeschwindigkeit auslesen in RPS aus Pedalen
float SpeedController::getRequestedRPS() {
    SpeedModeParameter mode = modiParameter[current_speed_mode];
    int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
    float maxRPS = convertKMHtoRPS(mode.maxSpeed);

    float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxRPS*100); 
    return toReturn/100; 
}

// gibt die aktuelle Geschwindigkeit in km/h zurück
float SpeedController::getCurrentKMH() {
    return convertRPStoKMh(getCurrentVelocity());
}

// gibt das aktuelle angeforderte Drehmoment in Nm zurück
float SpeedController::getRequestedNm() {
    SpeedModeParameter mode = modiParameter[current_speed_mode];
    int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
    float maxNM = mode.maxTorque;

    float toReturn = map(hallValue, 0, HALL_RESOLUTION, 0, maxNM*100); 
    return toReturn/100; 
}

float SpeedController::getCurrentNM() {
    return getVBusCurrent() * odrive->getParameterAsFloat("axis0.motor.config.torque_constant");
}

float SpeedController::convertRPStoKMh(float RPS) {
    float radiusM = RADIUS_WHEEL_CM / 100.0; // Konvertiere cm in m
    float circumferenceM  = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = circumferenceM * RPS; // m/s
    return speedMS * 3.6 *-1; // Konvertiere m/s in km/h
}

float SpeedController::convertKMHtoRPS(float KMH) {
    float radiusM = RADIUS_WHEEL_CM / 100.0; // Konvertiere cm in m
    float circumferenceM = M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = KMH / 3.6; // Konvertiere km/h in m/s
    return speedMS / circumferenceM; // Berechne RPS
}

// Gibt den Hallinput als Wert zwischen 0 und HALL_RESOLUTION (100) zurück
int SpeedController::getHallMappedValue(int gpio) {
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