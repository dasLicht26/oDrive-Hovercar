#include "SpeedController.h"
#include "Config.h"



// Aktuelle Batterielast auslesen in Ampere
float SpeedController::getVBusCurrent() {
    float amper = odrive->getParameterAsFloat("axis0.motor.current_control.Ibus") + odrive->getParameterAsFloat("axis1.motor.current_control.Ibus") + 0.1; // aktuelle amp
    return amper;
}

void SpeedController::reloadSettingsMenuValues() {
    if (LOCAL_DEBUG) {
        STANDARD_SETTING_ITEMS[0].current_value = 1.5f;
        STANDARD_SETTING_ITEMS[1].current_value = 2.5f;
    } else if (odrive != nullptr) {
        STANDARD_SETTING_ITEMS[0].current_value = getVelocityGain();
        STANDARD_SETTING_ITEMS[1].current_value = getVelocityIntegratorGain();
    }
    STANDARD_SETTING_ITEMS[2].current_value = getTorqueMinimum();
    STANDARD_SETTING_ITEMS[3].current_value = getTorqueSlope();
    STANDARD_SETTING_ITEMS[4].current_value = THROTTLE_CURVE_EXPONENT;
    STANDARD_SETTING_ITEMS[5].current_value = THROTTLE_LINEAR_BLEND;
    STANDARD_SETTING_ITEMS[6].current_value = THROTTLE_SMOOTHING_ALPHA;
}

void SpeedController::loadSavedSettings() {
    if (eeprom == nullptr) {
        return;
    }

    STANDARD_SETTING_ITEMS[2].current_value = eeprom->loadTorqueMinimum();
    STANDARD_SETTING_ITEMS[3].current_value = eeprom->loadTorqueSlope();
    THROTTLE_CURVE_EXPONENT = eeprom->loadThrottleCurveExponent();
    THROTTLE_LINEAR_BLEND = eeprom->loadThrottleLinearBlend();
    THROTTLE_SMOOTHING_ALPHA = eeprom->loadThrottleSmoothingAlpha();
    STANDARD_SETTING_ITEMS[4].current_value = THROTTLE_CURVE_EXPONENT;
    STANDARD_SETTING_ITEMS[5].current_value = THROTTLE_LINEAR_BLEND;
    STANDARD_SETTING_ITEMS[6].current_value = THROTTLE_SMOOTHING_ALPHA;
}

void SpeedController::applyRuntimeSettings() {
    float throttleCurve = STANDARD_SETTING_ITEMS[4].current_value;
    float throttleBlend = STANDARD_SETTING_ITEMS[5].current_value;
    float throttleSmoothing = STANDARD_SETTING_ITEMS[6].current_value;

    if (throttleCurve < 0.8f) {
        throttleCurve = 0.8f;
    } else if (throttleCurve > 3.0f) {
        throttleCurve = 3.0f;
    }

    if (throttleBlend < 0.0f) {
        throttleBlend = 0.0f;
    } else if (throttleBlend > 1.0f) {
        throttleBlend = 1.0f;
    }

    if (throttleSmoothing < 0.05f) {
        throttleSmoothing = 0.05f;
    } else if (throttleSmoothing > 1.0f) {
        throttleSmoothing = 1.0f;
    }

    STANDARD_SETTING_ITEMS[4].current_value = throttleCurve;
    STANDARD_SETTING_ITEMS[5].current_value = throttleBlend;
    STANDARD_SETTING_ITEMS[6].current_value = throttleSmoothing;
    THROTTLE_CURVE_EXPONENT = throttleCurve;
    THROTTLE_LINEAR_BLEND = throttleBlend;
    THROTTLE_SMOOTHING_ALPHA = throttleSmoothing;

    if (!LOCAL_DEBUG && odrive != nullptr) {
        odrive->setVelocityGain(STANDARD_SETTING_ITEMS[0].current_value);
        odrive->setVelocityIntegratorGain(STANDARD_SETTING_ITEMS[1].current_value);
    }
}

// Geschwindigkeit aktualisieren
void SpeedController::updateSpeed(){
    updateDirectionMode();
    udateIdleState();
    current_ampere = getVBusCurrent();
    switch(current_control_mode){
        case VELOCITY_CONTROL: {
            float requestedRPS = getRequestedRPS();
            
            // Wenn der aktuelle Strom das Limit überschreitet
            if (current_ampere > BAT_MAX_CURRENT) {
                // Reduziere die Geschwindigkeit schrittweise
                requestedRPS -= 0.9 * (current_ampere - BAT_MAX_CURRENT);
                if (requestedRPS < 0) {
                    requestedRPS = 0;  // Verhindert, dass die Geschwindigkeit negativ wird
                }
            }


            // Wenn die Geschwindigkeit unter dem Schwellenwert liegt und nicht zum aktuellen Modus passt, setze auf 0
            if (getSpeedMode() == MODE_R && getRequestedRPS() > 0) {
                requestedRPS = 0;
            } else if (getSpeedMode() != MODE_R && getRequestedRPS() < 0 && getCurrentVelocity() < (SPEED_OUTPUT_RPS_THRESHOLD * 3)) {
                requestedRPS = 0;
            }

            setTargetVelocity(requestedRPS);
            break;
        }
        case TORQUE_CONTROL: {
            float requestedNm = getRequestedNm();
            odrive->setTorque(requestedNm);
            break;
        }
    }
}



void SpeedController::udateIdleState(){
    if (getRequestedRPS() <= INPUT_REQUESTED_RPS_THRESHOLD && getRequestedRPS() >= -INPUT_REQUESTED_RPS_THRESHOLD && getCurrentVelocity() <= SPEED_OUTPUT_RPS_THRESHOLD && getCurrentVelocity() >= -SPEED_OUTPUT_RPS_THRESHOLD) {
        if (odrive -> getState(0) != AXIS_STATE_IDLE && odrive -> getState(1) != AXIS_STATE_IDLE) {
            stopMotorControl();
        }
    } else if(getRequestedRPS() > INPUT_REQUESTED_RPS_THRESHOLD || getRequestedRPS() < -INPUT_REQUESTED_RPS_THRESHOLD) {
        if (odrive -> getState(0) != AXIS_STATE_CLOSED_LOOP_CONTROL && odrive -> getState(1) != AXIS_STATE_CLOSED_LOOP_CONTROL) {
            startMotorControl();
        }
    }
}


// ODrive initialisieren/setup -> Ändert die Konfig dauerhaft
void SpeedController::saveODriveConfig() {
    
    applyRuntimeSettings();
    eeprom->saveTorqueSlope(STANDARD_SETTING_ITEMS[3].current_value);
    eeprom->saveTorqueMinimum(STANDARD_SETTING_ITEMS[2].current_value);
    eeprom->saveThrottleCurveExponent(STANDARD_SETTING_ITEMS[4].current_value);
    eeprom->saveThrottleLinearBlend(STANDARD_SETTING_ITEMS[5].current_value);
    eeprom->saveThrottleSmoothingAlpha(STANDARD_SETTING_ITEMS[6].current_value);

    if (LOCAL_DEBUG) {
        return;
    }

    if (odrive != nullptr) {

        odrive->clearErrors();
        odrive->setVelocityGain(STANDARD_SETTING_ITEMS[0].current_value);     // Proportionaler Anteil p-PID
        odrive->setVelocityIntegratorGain(STANDARD_SETTING_ITEMS[1].current_value);    // Integratoranteil i-PID

        switch (getControlMode()){
            case TORQUE_CONTROL: {
                odrive->setParameter("axis0.controller.config.control_mode", String((long)CONTROL_MODE_TORQUE_CONTROL));
                odrive->setParameter("axis1.controller.config.control_mode", String((long)CONTROL_MODE_TORQUE_CONTROL));
                break;
            }
            case VELOCITY_CONTROL: {
                odrive->setParameter("axis0.controller.config.control_mode", String((long)CONTROL_MODE_VELOCITY_CONTROL));
                odrive->setParameter("axis1.controller.config.control_mode", String((long)CONTROL_MODE_VELOCITY_CONTROL));
                break;
            }
        }

        //odrive->setParameter("axis0.motor.config.current_lim", BAT_MAX_CURRENT/2);
        //odrive->setParameter("axis1.motor.config.current_lim", BAT_MAX_CURRENT/2);

        //odrive->setParameter("axis0.motor.config.current_lim_margin", BAT_MAX_CURRENT_MARGIN/2);
        //odrive->setParameter("axis1.motor.config.current_lim_margin", BAT_MAX_CURRENT_MARGIN/2);
        setWatchdogEnabled(false);

        odrive->saveConfig();
        odrive->reboot();
        delay(1000);
        odrive->clearErrors();
        hardwareStartUpCheck();
        setWatchdogEnabled(true);
    }
}

// Alle Bewegungen stoppen
void SpeedController::stopMotorControl() {
    if (odrive != nullptr) {
        setTargetVelocity(0.0);
        odrive->setState(AXIS_STATE_IDLE, 0);
        odrive->setState(AXIS_STATE_IDLE, 1);
    }
    motor_active = false;
}

void SpeedController::stopCar() {
    if (odrive != nullptr) {
        // Bremse bis Stillstand
        odrive->setVelocity(0.0);
        // Warte bis Stillstand erreicht
        while (getCurrentKMH() != 0) {
            delay(30);
        }
    }
}

void SpeedController::calculateSpeedMode(){
    // Setze Speedmode je nach gedrückter Pedale
    int fw_pedal = getHallMappedValue(HALL_FW_PIN);
    int bw_pedal = getHallMappedValue(HALL_BW_PIN);
    if (fw_pedal < HALL_RESOLUTION/2 && bw_pedal < HALL_RESOLUTION/2) {
        setSpeedMode(MODE_1);
    } else if (fw_pedal > HALL_RESOLUTION/2 && bw_pedal < HALL_RESOLUTION/2) {
        setSpeedMode(MODE_2);
    } else if (fw_pedal < HALL_RESOLUTION/2 && bw_pedal > HALL_RESOLUTION/2) {
        setSpeedMode(MODE_3);
    } else if (fw_pedal > HALL_RESOLUTION/2 && bw_pedal > HALL_RESOLUTION/2) {
        setSpeedMode(MODE_4);
    } else {
        setSpeedMode(STANDARD_SPEED_MODE);
    }
    bool is_input = true;
    int input = 0;
    while (is_input)
    {   
        input = getHallMappedValue(HALL_FW_PIN) + getHallMappedValue(HALL_BW_PIN);
        if (input < 5) {
            is_input = false;
        } 
        Serial.print(is_input);
        delay(300);
    }
    
}


void SpeedController::hardwareStartUpCheck(){
    // Local Debugging --> überspringe ODrive-Initialisierung
    if (LOCAL_DEBUG) {
        return;
    }

    // Check ob ODrive verbunden ist, dazu wird eine einfache Abfrage gesendet
    while(odrive->getParameterAsFloat("vbus_voltage") == 0.0){
        delay(30);
    }
    float vBat = odrive->getParameterAsFloat("vbus_voltage");
    if(vBat <= V_BAT_MIN_START) {
        //displayManager -> setMenuState(ERROR_LOW_VOLTAGE); // Endlosschleife -> Batterie leer
    }
    // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive->getState(0) == AXIS_STATE_UNDEFINED) {
        delay(30);
    }
    // Warte bis odrive hochgefahren ist und State nicht mehr "undefined" ist
    while (odrive->getState(1) == AXIS_STATE_UNDEFINED) {
        delay(30);
    }

    motor_active = false;

}


void SpeedController::setWatchdogEnabled(bool enabled){
    if (odrive != nullptr) {
        odrive->setParameter("axis0.config.enable_watchdog", enabled);
        odrive->setParameter("axis1.config.enable_watchdog", enabled);
    }
}

void SpeedController::resetWatchdog(){
    if (odrive != nullptr) {
        odrive->resetWatchdog(0);
        odrive->resetWatchdog(1);
    }
}

void SpeedController::startMotorControl() {
    if (odrive != nullptr) {
        odrive->setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 0);
        odrive->setState(AXIS_STATE_CLOSED_LOOP_CONTROL, 1);
    }

    motor_active = true;
}


// Fehlercode-Mapping zu Text
String SpeedController::ODriveErrorToString(int error) {
    switch (error) {
        case 0: return "NONE";
        case 1: return "INVALID_STATE";
        case 2: return "UNDER_VOLTAGE";
        case 4: return "OVER_VOLTAGE";
        case 8: return "OVER_REGEN_CURRENT";
        case 16: return "OVER_CURRENT";
        case 32: return "BRAKE_DEADTIME";
        case 64: return "MOTOR_FAILED";
        case 128: return "SENSORLESS_ESTIMATOR";
        case 256: return "ENCODER_FAILED";
        case 512: return "CONTROLLER_FAILED";
        case 2048: return "WATCHDOG_EXPIRED";
        case 4096: return "MIN_ENDSTOP_PRESSED";
        case 8192: return "MAX_ENDSTOP_PRESSED";
        case 16384: return "ESTOP_REQUESTED";
        case 131072: return "HOMING_NO_ENDSTOP";
        case 262144: return "OVER_TEMP";
        case 524288: return "UNKNOWN_POSITION";

        default: return String(error);
    }
}



// Fehlerkontrolle, liefert eine Liste/struct mit Fehlern zurück
std::vector<ODriveErrors> SpeedController::getErrors() {
    std::vector<ODriveErrors> odrive_errors; // Liste mit Fehlern

    if (odrive != nullptr) {
        int systemError = odrive->getParameterAsInt("error");
        if (systemError != 0) {
            ODriveErrors a = {ODriveErrorToString(systemError), "System"};
            odrive_errors.push_back(a);
        }

        int axis0Error = odrive->getParameterAsInt("axis0.error");
        if (axis0Error != 0) {
            ODriveErrors b = {ODriveErrorToString(axis0Error), "Axis0"};
            odrive_errors.push_back(b);
        }

        int axis1Error = odrive->getParameterAsInt("axis1.error");
        if (axis1Error != 0) {
            ODriveErrors c = {ODriveErrorToString(axis1Error), "Axis1"};
            odrive_errors.push_back(c);
        }
    }

    return odrive_errors;
}

// Aktuelle Geschwindigkeit auslesen in RPS (nur positive Werte)
float SpeedController::getCurrentVelocity() {
    if (odrive != nullptr) {
        float vel_axis0 = odrive->getParameterAsFloat("axis0.encoder.vel_estimate") * -1;
        float vel_axis1 = odrive->getParameterAsFloat("axis1.encoder.vel_estimate");
        return (vel_axis0 + vel_axis1)/2; // Durchschnittsgeschwindigkeit der beiden Achsen
    }
    return 88.0;
}

float SpeedController::calculateTorque(float velocity) {
    //Quadratische Funktion, um das maximale Drehmoment abhängig von der Geschwindigkeit zu berechnen
    float min_torque = STANDARD_SETTING_ITEMS[2].current_value;
    float max_torque = getSpeedModeParameter().maxTorque;
    float torque_slope = STANDARD_SETTING_ITEMS[3].current_value;
    float torque = min_torque + torque_slope * pow(abs(velocity), 2); 

    // Begrenze das Drehmoment auf den maximalen Wert
    if (torque > max_torque) {
        torque = max_torque;
    }

    return torque;
}

float SpeedController::applyThrottleCurve(float normalized_input) {
    if (normalized_input > -INPUT_REQUESTED_RPS_THRESHOLD && normalized_input < INPUT_REQUESTED_RPS_THRESHOLD) {
        return 0.0f;
    }

    float sign = normalized_input < 0.0f ? -1.0f : 1.0f;
    float magnitude = fabs(normalized_input);
    if (magnitude > 1.0f) {
        magnitude = 1.0f;
    }

    float curved = pow(magnitude, THROTTLE_CURVE_EXPONENT);
    float blended = (THROTTLE_LINEAR_BLEND * magnitude) + ((1.0f - THROTTLE_LINEAR_BLEND) * curved);
    return sign * blended;
}

// Zielgeschwindigkeit setzen in RPS
void SpeedController::setTargetVelocity(float velocity) {
    float torque = calculateTorque(velocity);
    if (odrive != nullptr) {
        odrive->setVelocity(velocity, torque);
    }
}

// Zielgeschwindigkeit auslesen in RPS aus Pedalen
float SpeedController::getRequestedRPS() {
    SpeedModeParameter mode = modiParameter[current_speed_mode];
    int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
    float maxRPS = convertKMHtoRPS(mode.maxSpeed);

    float normalizedInput = (float)hallValue / (float)HALL_RESOLUTION;
    return applyThrottleCurve(normalizedInput) * maxRPS;
}

void SpeedController::updateDirectionMode() {
    if (current_speed_mode != MODE_R){
        temp_speed_mode = current_speed_mode;
    }
    if (getRequestedRPS() < -INPUT_REQUESTED_RPS_THRESHOLD && current_speed_mode != MODE_R && !motor_active) {
        setSpeedMode(MODE_R);
    } else if (current_speed_mode == MODE_R && !motor_active){
        setSpeedMode(temp_speed_mode);
    }
}

bool SpeedController::getMotorActive() {
    return motor_active;
}

// gibt das aktuelle angeforderte Drehmoment in Nm zurück
float SpeedController::getRequestedNm() {
    SpeedModeParameter mode = modiParameter[current_speed_mode];
    int hallValue = getHallMappedValue(HALL_FW_PIN) - getHallMappedValue(HALL_BW_PIN);
    float maxNM = mode.maxTorque;

    float normalizedInput = (float)hallValue / (float)HALL_RESOLUTION;
    return applyThrottleCurve(normalizedInput) * maxNM;
}

float SpeedController::getCurrentNM() {
    return getVBusCurrent() * odrive->getParameterAsFloat("axis0.motor.config.torque_constant");
}

float SpeedController::convertRPStoKMh(float RPS) {
    float radiusM = RADIUS_WHEEL_CM / 100.0; // Konvertiere cm in m
    float circumferenceM  = 2.0 * M_PI * radiusM; // Berechne den Umfang in Metern
    float speedMS = circumferenceM * RPS; // m/s
    return speedMS * 3.6 *-1; // Konvertiere m/s in km/h
}

float SpeedController::convertKMHtoRPS(float KMH) {
    float radiusM = RADIUS_WHEEL_CM / 100.0; // Konvertiere cm in m
    float circumferenceM = 2.0 * M_PI * radiusM; // Berechne den Umfang in Metern
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
    // verhnidern, dass Werte über HALL_RESOLUTION zurückgegeben werden
    if(toReturn >= HALL_RESOLUTION){
        toReturn = HALL_RESOLUTION;
    }

    float* filteredValue = nullptr;
    if (gpio == HALL_FW_PIN) {
        filteredValue = &hall_fw_filtered;
    } else if (gpio == HALL_BW_PIN) {
        filteredValue = &hall_bw_filtered;
    }

    if (filteredValue == nullptr) {
        return toReturn;
    }

    if (*filteredValue < 0.0f) {
        *filteredValue = (float)toReturn;
    } else {
        *filteredValue += THROTTLE_SMOOTHING_ALPHA * ((float)toReturn - *filteredValue);
    }

    return (int)(*filteredValue + 0.5f);
}
