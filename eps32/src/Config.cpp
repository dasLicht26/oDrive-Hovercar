#include "Config.h"
#include <Arduino.h>

/*
==========================================
==     SETTINGS - HARDWARE - ESP32/Dispaly      ==
==========================================
*/

// GPIO-Pins für Knopf 1 und 2
const int BUTTON_1 = 13; // Button 1 (GPIO 13)
const int BUTTON_2 = 14; // Button 2 (GPIO 14)

const int BUTTON_UP = 16;   // GPIO des LED Pannels, wird als Button verwendet (GPIO 16)
const int BUTTON_DOWN = 17; // GPIO des LED Pannels, wird als Button verwendet (GPIO 17)
const int BUTTON_OK = 12;   // GPIO des LED Pannels, wird als Button verwendet (GPIO 12)

// GPIO-Pins für I2C-Display
const int OLED_SDA = 21; // (GPIO 21)
const int OLED_SCL = 22; // (GPIO 22)

// GPIO-Pins für oDrive UART Kommunikation
const int  ODRIVE_RX = 10;  // verwendeter GPIO für RX UART-Bus des ESP32 (GPIO 10)
const int  ODRIVE_TX = 9;   // verwendeter GPIO für TX UART-Bus des ESP32 (GPIO 9)

// GPIO-Pins für HALL Sensoren (Pedale)
const int HALL_BW_PIN = 35; // HALL2 Backward Pedale analoger Input (GPIO 35)
const int HALL_FW_PIN = 34; // HALL1 Forward Pedale analoger Input (GPIO 34)

// Konfig HALL-Sensoren (Pedale)
const float HALL_ANALOG_MAX = 3000; // Analog gelesene Spannung wenn Pedal komplett durchgedrückt wird (3.0V = 3000)
const float HALL_ANALOG_MIN = 540; // Analog gelesene Spannung wenn Pedal nicht betätigt wird (0.54V = 540)
const int HALL_RESOLUTION = 100; // Anzahl der Schritte zwischen Pedal gedrückt/nicht betätigt (100)

// Konfig Batterie 
const int V_BAT_MAX = 41; // 100% Ladung der Akkus, Werte darüber werden auf 41 abgeschnitten. (41 Volt)
const int V_BAT_MIN = 32; // 0% Ladung des Akkus, das HoverCar nimmt kein Gas mehr an wenn es bereits eingeschalten ist (Es bremmst auf 0). (32 Volt)
const int V_BAT_MIN_START = 34; // Minimale Akkuladung um odrive in AXIS_STATE_CLOSED_LOOP_CONTROL zu versetzen. (darunter lässt sich das HooverCar nicht mehr einschalten) (34 Volt)
const float BAT_MAX_CURRENT = 24.0; // Maximaler Strom bei Dauerbelastung in Amper (20A)
const float BAT_MAX_CURRENT_MARGIN = 4.0; // Maximaler zusätzlicher Peak in Amper (<1sek) (Passiert durch die PI-Steuerung) (4A)

// Definiere ESP32 GPIO-Pins und UART-Schnittstelle zu oDrive
const int ODRIVE_BAUD_RATE = 115200; // Baudrate zur Kommunikation mit oDrive (115200)
const int ODRIVE_UART = 1; // verwendeter UART-Bus des ESP32 (UART1)

// Display Einstellungen
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
const int OLED_RESET = -1;

/*
==========================================
==     SETTINGS - SOFTWARE - ESP32          ==
==========================================
*/

const float RADIUS_WHEEL_CM = 21.0; // Definiere Reifendurchmesser in cm (Für Geschwindigkeitberechnung in Kmh)

ControlMode STANDARD_CONTROL_MODE = VELOCITY_CONTROL; // Standard Steuerungsmodus
SpeedMode STANDARD_SPEED_MODE = MODE_1; // standard Geschwindigkeitsmodus
MenuState STANDARD_MENUE = MENU_MAIN; // standard Menüzustand (Display)
bool DEBUG_MODE_AKIV = false; // Debugmodus aktivieren

// Parameter der einzelnen Speedmodi (Name (für Display), maxSpeed, maxTorque)
const SpeedModeParameter modiParameter[] = {
  {"1", 5.0, 10.0},
  {"2", 8.0, 6.0},
  {"3", 12.0, 8.0},
  {"4", 16.0, 999.0},
  {"R", -4.0, -5.0},
};


// Menüpunkte für das Display
const MenuItem menu_settings_items[]= {
    {"Vel Gain", 0.0, 0.1, true, false},
    {"Vel Int Gain", 0.0, 0.05, true, false},
    {"Control Mode", 0.0, 1.0, true, false},
    {"Save Settings", 0.0, 0.0, false, false}
};