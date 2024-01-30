#ifndef ODRIVEINTERFACE_H
#define ODRIVEINTERFACE_H

#include <HardwareSerial.h>

class ODriveInterface {
public:
    ODriveInterface(HardwareSerial& serial);
    void begin(int baudRate, int rxPin, int txPin);
    float readVelocity();
    void writeVelocity(float velocity);
    float readBatteryVoltage();
    float readCurrent();

private:
    HardwareSerial& odrive_serial;
    String sendCommand(String command);
};

#endif
