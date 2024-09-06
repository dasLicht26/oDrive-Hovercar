#ifndef Constants_h
#define Constants_h


/*
==========================================
==     SETTINGS - HARDWARE - ESP32/BATT      ==
==========================================
*/

// GPIO-Pins für Knopf 1 und 2
#define BUTTON_1 13 // verwendeter GPIO für Button 1
#define BUTTON_2 14 // verwendeter GPIO für Button 2

#define BUTTON_UP 16 // GPIO des LED Pannels, wird als Button verwendet
#define BUTTON_DOWN 17 // GPIO des LED Pannels, wird als Button verwendet
#define BUTTON_OK 12// GPIO des LED Pannels, wird als Button verwendet

// GPIO-Pins für I2C-Display
#define OLED_SDA 21 // GPIO21
#define OLED_SCL 22 // GPIO22

// GPIO-Pins für oDrive UART Kommunikation
const int  ODRIVE_RX = 10; // verwendeter GPIO für RX UART-Bus des ESP32 (GPIO-10)
const int  ODRIVE_TX = 9; // verwendeter GPIO für TX UART-Bus des ESP32 (GPIO-9)

// GPIO-Pins für HALL Sensoren (Pedale)
const int HALL_BW_PIN = 35; // GPIO35 = HALL2 Backward Pedale analoger Input
const int HALL_FW_PIN = 34; // GPIO34 = HALL1 Forward Pedale analoger Input

// Konfig HALL-Sensoren (Pedale)
const float HALL_ANALOG_MAX = 3000; // Analog gelesene Spannung wenn Pedal komplett durchgedrückt wird (3.0V = 3000)
const float HALL_ANALOG_MIN = 540; // Analog gelesene Spannung wenn Pedal nicht betätigt wird (0.54V = 540)
const int HALL_RESOLUTION = 100; // Anzahl der Schritte zwischen Pedal gedrückt/nicht betätigt

// Konfig Batterie 
const int V_BAT_MAX = 41; // 41 Volt = 100% Ladung der Akkus, Werte darüber werden auf 41 abgeschnitten.
const int V_BAT_MIN = 32; // 32 Volt = 0% Ladung des Akkus, das HoverCar nimmt kein Gas mehr an wenn es bereits eingeschalten ist (Es bremmst auf 0).
const int V_BAT_MIN_START = 34; // Minimale Akkuladung um odrive in AXIS_STATE_CLOSED_LOOP_CONTROL zu versetzen. (darunter lässt sich das HooverCar nicht mehr einschalten)
const float BAT_MAX_CURRENT = 20.0; // Maximaler Strom bei Dauerbelastung in Amper
const float BAT_MAX_CURRENT_MARGIN = 4.0; // Maximaler zusätzlicher Peak in Amper (<1sek) (Passiert durch die PI-Steuerung)

// Definiere ESP32 GPIO-Pins und UART-Schnittstelle zu oDrive
const int ODRIVE_BAUD_RATE = 115200; // Baudrate zur Kommunikation mit oDrive (115200)
const int ODRIVE_UART = 1; // verwendeter UART-Bus des ESP32 (UART1)


/*
==========================================
==     SETTINGS - SOFTWARE          ==
==========================================
*/

const float RADIUSCM = 21.0; // Definiere Reifendurchmesser (Für Geschwindigkeitberechnung in Kmh) in cm

// Errors
struct ODriveErrors {
    int errorCode;
    String source;
};


//Kontroll Modi
enum ControlMode { 
    VELOCITY_CONTROL, // Geschwindigkeitssteuerung (Ziel ist eine bestimmte RPS oder KMH)
    TORQUE_CONTROL  // Drehmomentsteuerung (Ziel ist eine bestimmte Nm)
};


// Definiere die verfügbaren Speedmodi als enum
enum SpeedMode {
  MODE_1 = 0,
  MODE_2 = 1,
  MODE_3 = 2,
  MODE_4 = 3,
  MODE_R = 4
};

// Mögliche-Menüzustände
enum MenuState {
    ERROR_LOW_VOLTAGE, // Geringe Batteriespannung 
    ERROR_ODRIVE, // oDrive hat Fehler festgestellt
    MENU_MAIN,  // Hauptmenue -> Standardansicht
    MENU_DEBUG, // Hidden -> Debug Ausgabe 
    MENU_ADJUST_MAIN, 
    MENU_ADJUST_VEL_GAIN,
    MENU_ADJUST_VEL_INTEGRATOR_GAIN,
    MENU_ADJUST_CONTROL_MODE,
    MENU_SAVE_SETTINGS
};

// Dauerhaft gespeicherte Einstellungen (werden nach neustart erhaten)
struct Settings {
  SpeedMode speedMode;
  ControlMode controlMode;
  float velocityGain;
  float velocityIntegratorGain;
}; //settingsLayout;

// Struktur für die Parameter der Speedmodi
struct SpeedModeParameter {
  const char* name; // Name des Modus
  float maxSpeed;   // Geschwindigkeitswert
  float maxTorque;  // Drehmoment in Nm
};



// Definiere die Parameter für jeden Modus
// {str: Name des Modus, int:max Km/h, int:max Drehmoment in Nm}
const SpeedModeParameter modiParameter[] = {
  {"1", 5.0, 10.0},
  {"2", 8.0, 6.0},
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
