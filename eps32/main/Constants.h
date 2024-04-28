#ifndef KONSTANTEN_H
#define KONSTANTEN_H

/*
==========================================
==     SETTINGS - HARDWARE/GPIO         ==
==========================================
*/

// GPIO-Pins für Knopf 1 und 2
#define BUTTON_1 13 // verwendeter GPIO für Button 1
#define BUTTON_2 14 // verwendeter GPIO für Button 2

// GPIO-Pins für I2C-Display
#define OLED_SDA 21 // GPIO21
#define OLED_SCL 22 // GPIO22

// GPIO-Pins für oDrive UART Kommunikation
const int  ODRIVE_RX = 10; // verwendeter GPIO für RX UART-Bus des ESP32 (GPIO-10)
const int  ODRIVE_TX = 9; // verwendeter GPIO für TX UART-Bus des ESP32 (GPIO-9)

// GPIO-Pins für HALL Sensoren (Pedale)
const int HALL_BW_PIN = 35; // GPIO35 = HALL2 Backward analoger Input
const int HALL_FW_PIN = 34; // GPIO34 = HALL1 Forward  analoger Input

// Konfig HALL-Sensoren (Pedale)
const float HALL_ANALOG_MAX = 3000; // Analog gelesene Spannung wenn Pedal komplett durchgedrückt wird (3.0V = 3000)
const float HALL_ANALOG_MIN = 540; // Analog gelesene Spannung wenn Pedal nicht betätigt wird (0.54V = 540)
const int HALL_RESOLUTION = 100; // Anzahl der Schritte zwischen Pedal gedrückt/nicht betätigt

// Konfig Batterie 
const int V_BAT_MAX = 41; // 41 Volt = 100% Ladung der Akkus, Werte darüber werden auf 41 abgeschnitten.
const int V_BAT_MIN = 32; // 32 Volt = 0% Ladung des Akkus, das HoverCar nimmt kein Gas mehr an wenn es bereits eingeschalten ist (Es bremmst auf 0).
const int V_BAT_MIN_START = 34; // Minimale Akkuladung um odrive in AXIS_STATE_CLOSED_LOOP_CONTROL zu versetzen. (darunter lässt sich das HooverCar nicht mehr einschalten)

// Definiere ESP32 GPIO-Pins und UART-Schnittstelle zu oDrive
const int ODRIVE_BAUD_RATE = 115200; // Baudrate zur Kommunikation mit oDrive (115200)
const int ODRIVE_UART = 1; // verwendeter UART-Bus des ESP32 (UART1)
/*
==========================================
==     SETTINGS - SPEEDCONTROL          ==
==========================================
*/

const float RADIUSCM = 21.0; // Definiere Reifendurchmesser (Für Geschwindigkeitberechnung in Kmh) in cm

//Kontroll Modi
enum ControlMode{
  SPEED_CONTROL = 0,
  TORQUE_CONTROL = 1 
};

// Struktur für die Parameter der Speedmodi
struct SpeedModusParameter {
  const char* name; // Name des Modus
  float maxSpeed;   // Geschwindigkeitswert
  float maxTorque;  // Beschleunigungswert
};

// Definiere die verfügbaren Speedmodi als enum
enum SpeedMode {
  MODE_1,
  MODE_2,
  MODE_3,
  MODE_4,
  MODE_R
};

// Definiere die Parameter für jeden Modus
// {str: Name des Modus, int:max Km/h, int:max Drehmoment}
const SpeedModusParameter modiParameter[] = {
  {"1", 5.0, 2.0},
  {"2", 8.0, 5.0},
  {"3", 12.0, 8.0},
  {"4", 16.0, 999.0},
  {"R", -4.0, -5.0},
};


/*
==========================================
==     SETTINGS - DISPLAY               ==
==========================================
*/

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

#endif
