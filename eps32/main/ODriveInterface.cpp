#include "ODriveInterface.h"

ODriveInterface::ODriveInterface(HardwareSerial& serial) : odrive_serial(serial) {}

void ODriveInterface::begin(int baudRate, int rxPin, int txPin) {
    odrive_serial.begin(baudRate, SERIAL_8N1, rxPin, txPin);
}

String ODriveInterface::sendCommand(String command) {
    odrive_serial.println(command);
    long startMillis = millis();
    while (!odrive_serial.available()) {
        if (millis() - startMillis > 500) {
            return "Timeout";
        }
        delay(10);
    }
    return odrive_serial.readStringUntil('\n');
}

float ODriveInterface::readVelocity() {
    String response = sendCommand("r axis0.encoder.vel_estimate");
    return response.toFloat();
}

void ODriveInterface::writeVelocity(float velocity) {
    sendCommand("v axis0 " + String(velocity));
}

float ODriveInterface::readBatteryVoltage() {
    String response = sendCommand("r vbus_voltage");
    return response.toFloat();
}

float ODriveInterface::readCurrent() {
    String response = sendCommand("r axis0.motor.current_control.Iq_measured");
    return response.toFloat();
}

// Implementieren Sie weitere Methoden ähnlich wie readVelocity() für Geschwindigkeit schreiben, Batteriespannung lesen, usw.
