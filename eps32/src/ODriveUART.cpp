// Author: ODrive Robotics Inc.
// License: MIT
// Documentation: https://docs.odriverobotics.com/v/latest/guides/arduino-uart-guide.html

#include "Arduino.h"
#include "ODriveUART.h"

static const int kMotorNumber = 0;
static const int timeout = 1000;

// Print with stream operator
template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

ODriveUART::ODriveUART(Stream& serial)
    : serial_(serial) {}

void ODriveUART::clearErrors() {
    serial_ << "sc\n";
}

void ODriveUART::saveConfig() {
    serial_ << "ss\n";
}

void ODriveUART::reboot() {
    serial_ << "sr\n";
}

void ODriveUART::setVelocity(float velocity) {
    serial_ << "v " << 0  << " " << velocity * -1 <<  "\n";
    serial_ << "v " << 1  << " " << velocity << "\n";
}

void ODriveUART::setVelocity(float velocity, float torque) {
    serial_ << "v " << 0  << " " << velocity * -1 <<" " << torque * -1<< "\n";
    serial_ << "v " << 1  << " " << velocity << " " << torque << "\n";
}

void ODriveUART::setTorque(float torque) {
    serial_ << "c " << 0 << " " << torque * -1 << "\n";
    serial_ << "c " << 1 << " " << torque << "\n";
}

void ODriveUART::resetWatchdog(int axis) {
    serial_ << "u " << axis << "\n";
}

void ODriveUART::trapezoidalMove(float position, int axis) {
    serial_ << "t " << axis << " " << position << "\n";
}

ODriveFeedback ODriveUART::getFeedback(int axis) {
    // Flush RX
    while (serial_.available()) {
        serial_.read();
    }

    serial_ << "f " << axis << "\n";

    String response = readLine(timeout);

    // Überprüfe, ob eine Antwort empfangen wurde
    if (response.length() == 0) {
        return {0.0f, 0.0f}; // Gebe einen Fehlerwert zurück
    }

    int spacePos = response.indexOf(' ');
    if (spacePos >= 0) {
        return {
            response.substring(0, spacePos).toFloat(),
            response.substring(spacePos+1).toFloat()
        };
    } else {
        return {0.0f, 0.0f};
    }
}

String ODriveUART::getParameterAsString(const String& path) {
    serial_ << "r " << path << "\n";
    String response = readLine(timeout); // Setze ein Timeout 

    // Überprüfe, ob eine Antwort empfangen wurde
    if (response.length() == 0) {
        return "ERROR"; // Rückgabe eines leeren Strings als Fehlerindikator
    }
    return response;
}


void ODriveUART::setParameter(const String& path, const String& value) {
    serial_ << "w " << path << " " << value << "\n";
}

void ODriveUART::setParameter(const String& path, float value) {
    serial_ << "w " << path << " " << value << "\n";
}

void ODriveUART::setState(ODriveAxisState requested_state) {
        setParameter("axis0.requested_state", String((long)requested_state));
        setParameter("axis1.requested_state", String((long)requested_state));
}

void ODriveUART::setState(ODriveAxisState requested_state, int axis) {
    if (axis == 0) {
        setParameter("axis0.requested_state", String((long)requested_state));
    } else if (axis == 1) {
        setParameter("axis1.requested_state", String((long)requested_state));
    }
}

 void ODriveUART::setVelocityGain(float velocity_gain){
    setParameter("axis0.controller.config.vel_gain", velocity_gain);
    setParameter("axis1.controller.config.vel_gain", velocity_gain);
}

 void ODriveUART::setVelocityIntegratorGain(float velocity_integrator_gain){
    setParameter("axis0.controller.config.velocity_integrator_gain", velocity_integrator_gain);
    setParameter("axis1.controller.config.velocity_integrator_gain", velocity_integrator_gain);
}

 void ODriveUART::setControlMode(const String& control_mode){
    setParameter("axis0.controller.config.control_mode", control_mode);
    setParameter("axis1.controller.config.control_mode", control_mode);
}

ODriveAxisState ODriveUART::getState(int axis) {
    if (axis == 0) {
        return (ODriveAxisState)getParameterAsInt("axis0.current_state");
    } else if (axis == 1) {
        return (ODriveAxisState)getParameterAsInt("axis1.current_state");
    }
}

String ODriveUART::readLine(unsigned long timeout_ms) {
    String str = "";
    unsigned long timeout_start = millis();
    delay(10);

    for (;;) {
        while (!serial_.available()) {
            if (millis() - timeout_start >= timeout_ms) {
                Serial.println("Timeout");
                return str;
            }
            delay(3);
        }
        char c = serial_.read();
        if (c == '\n')
            break;
        str += c;
    }

    return str;
}
